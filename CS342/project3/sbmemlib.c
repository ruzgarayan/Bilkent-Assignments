#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

// Define a name for your shared memory; you can give any name that start with a slash character; it will be like a filename.
#define SHAREDMEM_NAME "/SBMEMLIB"

#define MAX_PROCESSES 10

#define MIN_SHARED_SEGMENT_SIZE 32768
#define MAX_SHARED_SEGMENT_SIZE 262144

#define MIN_SHARED_REQUEST_SIZE 128
#define MAX_SHARED_REQUEST_SIZE 4096

#define MAX_POSSIBLE_ORDER 12 //Because (MIN_SHARED_REQUEST_SIZE) * (2^12) = (MAX_SHARED_SEGMENT_SIZE)

// Define semaphore(s)

#define MUTEX_SEMAPHORE_NAME "/mutex_semaphore"
sem_t *sem_mutex;

// Define your stuctures and variables. 

//This block info is put into the beginning of every memory block.
struct blockInfo {
	//Starting address of the block, taking 0 as the beginning of shared memory.
	int address;

	//This information is used to represent the size of the block.
	//order = 0 means that the size is (MIN_SHARED_REQUEST_SIZE * 2^0) = 128
	//order = n means that the size is 128 * 2^n
	int order;

	//ID of the assigned processor,
	//0 if free
	pid_t processId;

	//This points to the address of the next block in the free (linked) list,
	//Head of these free (linked) lists are stored in the managementTable struct.
	//Its value is not important if not in the list.
	int nextBlockAddress;
};

struct processInfo {
	pid_t pid;
	void *shm_start;
};

//This table is put in the beginning of the shared memory.
struct managementTable {
	//Size of the shared segment in bytes exluding the management table part.
	int segmentsize;

	//The order of the largest memory block in the buddy tactic.
	int max_order;

	//Heads of the linked lists that hold the free memory blocks of each order.
	//e.g. freeBlockPointers[0] is the head of the linked list with memory blocks of order 0.
	//Linked list works with addresses of the memory blocks. -1 means the end of the list.
	int freeBlockPointers[MAX_POSSIBLE_ORDER];

	//There are MAX_PROCESSES slots for the concurrent usage of the memory segment.
	//Each element of the array holds the pid and mapped start of the shared memory segment of the using process.
	struct processInfo processes[MAX_PROCESSES];
};

//Library does not persist between processes.
//So for every single process, we can keep its mapped shared memory starting address as a global variable here.
//When sbm_open is called, the library will maps the shared segment
//to the virtual address space of the process and stores it here for the next calls of sbm_alloc/sbm_free.
void *shm_start_process; 



void open_semaphore()
{
	sem_mutex = sem_open(MUTEX_SEMAPHORE_NAME, O_RDWR);
	if (sem_mutex < 0) {
		perror("Could not open semaphore, exiting.\n");
		exit (1); 
	}
}

void close_semaphore()
{
	sem_close(sem_mutex);
}

int open_and_map(void **shm_start_)
{
	int fd;	
	void *shm_start;
	fd = shm_open(SHAREDMEM_NAME, O_RDWR, 0660);
	if (fd < 0)
		return 0;

	shm_start = mmap(NULL, sizeof(struct managementTable), 
			 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shm_start < 0)
		return 0;

	struct managementTable *MT = (struct managementTable *) shm_start;
	int segmentsize = MT->segmentsize;

	shm_start = mmap(NULL, segmentsize + sizeof(struct managementTable), 
			 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shm_start < 0)
		return 0;
	close(fd);

	*(shm_start_) = shm_start;

	//printf("Opened MT with segmentsize %d and max_order %d\n", MT->segmentsize, MT->max_order);

	return 1;
}





int sbmem_init(int segmentsize)
{
	int max_order;
	int fd;	
	void *shm_start;

    if (segmentsize > MAX_SHARED_SEGMENT_SIZE || segmentsize < MIN_SHARED_SEGMENT_SIZE) {
		return -1;
	}

	max_order = 0;
	int power2 = MIN_SHARED_REQUEST_SIZE;
	while (power2 < segmentsize) {
		power2 *= 2;
		max_order += 1;
	}
	if (power2 != segmentsize) {
		//printf("Segment size is not a power of 2.\n");
		return -1;
	}

	//Check if shared memory already exists
	fd = shm_open(SHAREDMEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0660);
	if (fd < 0) { /* Shared memory already exists, so delete it and create new */
		if (shm_unlink(SHAREDMEM_NAME) < 0) {
			//printf("Could not unlink the already existing shared memory.\n");
    		return -1; 
		}
		//else printf ("Unlinked the already existing shared memory.\n");


		//Create new shared memory
		fd = shm_open(SHAREDMEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0660);
		if (fd < 0) {
			//printf("Error while creating the shared memory.\n");
			return -1; 
		}
		//else printf("Created the shared memory.\n");
	}
	//else printf("Created the shared memory.\n");

	ftruncate(fd, segmentsize + sizeof(struct managementTable));

	shm_start = mmap(NULL, segmentsize + sizeof(struct managementTable), 
			 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	struct managementTable* MT = (struct managementTable *) shm_start;
	MT->segmentsize = segmentsize;
	MT->max_order = max_order;

	for (int i = 0; i < MAX_POSSIBLE_ORDER; i++)
	{
		MT->freeBlockPointers[i] = -1;
	}
	MT->freeBlockPointers[max_order] = 0;

	for (int i = 0; i < MAX_PROCESSES; i++)
	{
		MT->processes[i].pid = 0;
		MT->processes[i].shm_start = NULL;
	}

	struct blockInfo* initalBlockInfo = (struct blockInfo *) (shm_start + sizeof(struct managementTable));
	initalBlockInfo->address = 0;
	initalBlockInfo->order = max_order;
	initalBlockInfo->processId = 0;
	initalBlockInfo->nextBlockAddress = -1;

	//Create the semaphore
	sem_unlink (MUTEX_SEMAPHORE_NAME);
	sem_mutex = sem_open(MUTEX_SEMAPHORE_NAME, O_CREAT, 0660, 1);
	if (sem_mutex < 0) {
		perror("Could not create semaphore, exiting\n");
		exit (1); 
	}

	sem_close(sem_mutex);
	return 0;
}

int sbmem_remove()
{
	if (shm_unlink(SHAREDMEM_NAME) < 0)
	{
		return -1;
	}
	else
	{
		//Remove the semaphore too
		sem_unlink (MUTEX_SEMAPHORE_NAME);
	    return 0; 
	}
}

int sbmem_open()
{
	open_semaphore();
	void *shm_start;
	struct managementTable *MT;
	sem_wait(sem_mutex);

	int opened = open_and_map(&shm_start);
	if (!opened)
	{
		printf("Error while opening and mapping the shared memory, exiting.\n");
		sem_post(sem_mutex);
		exit(1);
	}
	MT = (struct managementTable *) shm_start;

	pid_t pid = getpid();
	for (int i = 0; i < MAX_PROCESSES; i++)
	{
		if (MT->processes[i].pid == 0)
		{
			MT->processes[i].pid = pid;
			MT->processes[i].shm_start = shm_start;
			shm_start_process = shm_start;

			sem_post(sem_mutex);
			return 0;
		}
	}

	sem_post(sem_mutex);
	close_semaphore();
    return -1; 
}

//For the given block info, splits that block into two buddies.
void splitMemoryBlock(void *shm_start, struct managementTable *MT, struct blockInfo *bInfo)
{
	int address = bInfo->address;
	int order = bInfo->order;

	//Assuming that the input to this function is always the first element from these linked lists.
	//Then, we can easily remove this block from the free list of its order.
	MT->freeBlockPointers[order] = bInfo->nextBlockAddress;

	//Decrease the order of this block.
	order--;
	bInfo->order = order;
	//The starting address remains the same.

	//Create the buddy block.
	int power2order = MIN_SHARED_REQUEST_SIZE;
	for (int i = 0; i < order; i++) power2order *= 2;

	int buddyAddress = address + power2order;
	struct blockInfo *buddyBInfo = (struct blockInfo *)(shm_start + buddyAddress);
	buddyBInfo->address = buddyAddress;
	buddyBInfo->order = order;
	buddyBInfo->processId = -1;

	//Add these two into the linked list of current order.
	bInfo->nextBlockAddress = buddyAddress;
	buddyBInfo->nextBlockAddress = MT->freeBlockPointers[order];
	MT->freeBlockPointers[order] = address;

	//printf("Created buddy block at address %d with order %d\n", buddyAddress, order);
} 

//If the buddy of the given memory block is free,
//combine these into a larger order memory block, go on combining recursively.
void combineBuddies(void *shm_start, struct managementTable *MT, struct blockInfo *bInfo)
{
	int address = bInfo->address;
	int order = bInfo->order;
	int max_order = MT->max_order;

	//printf("In combineBuddies method for address %d and order %d\n", address, order);

	if (order == max_order) //Already the largest possible block, so don't do anything
	{
		MT->freeBlockPointers[order] = address;
		return;
	}

	//Find the buddy's block info 
	int power2order = MIN_SHARED_REQUEST_SIZE;
	for (int i = 0; i < order ; i++) power2order *= 2;

	//We can find the buddy's address directly using the observations in 
	// Knuth's Art of Programming book.
	int buddyAddress;
	if (address % (power2order * 2) == 0)
		buddyAddress = address + power2order;
	else
		buddyAddress = address - power2order;
	struct blockInfo *buddyBInfo = (struct blockInfo *)(shm_start + buddyAddress);

	if (buddyBInfo->processId >= 0) { //The buddy is currently used, so don't combine.
		bInfo->nextBlockAddress = MT->freeBlockPointers[order];
		MT->freeBlockPointers[order] = address;
	}
	else { //The buddy is free, so we can combine.

		//First, remove the buddy from the free list of management table.
		if (MT->freeBlockPointers[order] == buddyAddress) {
			MT->freeBlockPointers[order] = buddyBInfo->nextBlockAddress;
		}
		else {
			//We need to find the previous block in the free list in order the update its next pointer.
			int nextAddress = MT->freeBlockPointers[order];
			struct blockInfo *nextBInfo = (struct blockInfo *)(shm_start + nextAddress);
			while (nextBInfo->nextBlockAddress != buddyAddress)
			{
				nextAddress = nextBInfo->nextBlockAddress;
				nextBInfo = (struct blockInfo *)(shm_start + nextAddress);
			}

			nextBInfo->nextBlockAddress = buddyBInfo->nextBlockAddress;
		}

		//Then, create the higher order block and add it to the free list of management table.
		int newBlockAddress = (address < buddyAddress) ? address : buddyAddress;
		struct blockInfo *newBInfo = (struct blockInfo *)(shm_start + newBlockAddress);
		newBInfo->address = newBlockAddress;
		newBInfo->order = order + 1;
		newBInfo->processId = 0;
		newBInfo->nextBlockAddress = -1;

		combineBuddies(shm_start, MT, newBInfo);
	}
}

void *sbmem_alloc (int alloc_size)
{
	if (alloc_size > MAX_SHARED_REQUEST_SIZE || alloc_size < MIN_SHARED_REQUEST_SIZE) {
		return (NULL);
	}

	struct managementTable *MT = (struct managementTable *) shm_start_process;
	void *shm_start = shm_start_process + sizeof(struct managementTable);

	sem_wait(sem_mutex);
	int max_order = MT->max_order;

	//Also need to consider the overhead from block info.
	int size = alloc_size + sizeof(struct blockInfo);
	int alloc_order = 0;
	int power2 = MIN_SHARED_REQUEST_SIZE;
	while (power2 < size)
	{
		power2 *= 2;
		alloc_order += 1;
	}

	int order = alloc_order;
	//Find the smallest order that has a free block.
	while (order <= max_order && MT->freeBlockPointers[order] == -1)
	{
		order++;
	}

	//Take the first free block from the free list of that order.
	int availableBlockAddress = MT->freeBlockPointers[order];
	struct blockInfo *availableBlockInfo = (struct blockInfo *)(shm_start + availableBlockAddress);

	//If there is no free block, then the loop above ends with max_order + 1.
	if (order == max_order + 1)
	{
		sem_post(sem_mutex);
		return (NULL);
	}
	//else
	//	printf("at address %d with order %d is suitable \n", availableBlockInfo->address, availableBlockInfo->order);

	//Split the found block into two buddies until we obtain a block of desired size.
	while (order > alloc_order)
	{
		splitMemoryBlock(shm_start, MT, availableBlockInfo);
		order--;
		//printf("new order %d\n", order);
	}

	availableBlockInfo->processId = getpid();
	MT->freeBlockPointers[order] = availableBlockInfo->nextBlockAddress;
	//printf("Allocated address %d with order %d\n", availableBlockAddress, availableBlockInfo->order);

	sem_post(sem_mutex);
	return (shm_start + availableBlockAddress + sizeof(struct blockInfo));
}


void sbmem_free (void *p)
{
	struct managementTable *MT = (struct managementTable *) shm_start_process;
	void *shm_start = shm_start_process + sizeof(struct managementTable);

	sem_wait(sem_mutex);


	//Shift left by sizeof(struct blockInfo) to get to the start of blockInfo.
	struct blockInfo *freeBlockInfo = (struct blockInfo *)(p - sizeof(struct blockInfo));

	//After freeing a block, buddies will be combinedd into larger order block.
	//The method below does all the combining recursively.
	combineBuddies(shm_start, MT, freeBlockInfo);

	//int order = freeBlockInfo->order;
	//int address = freeBlockInfo->address;
	//int process = freeBlockInfo->processId;
	//printf("Trying to free the block at address %d with order %d, allocated by the process %d\n", address, order, process);

	sem_post(sem_mutex);
}

int sbmem_close()
{
    struct managementTable *MT = (struct managementTable *) shm_start_process;

	sem_wait(sem_mutex);
	pid_t pid = getpid();

	//TEST WRITE
	/*
	char *shrseg = (char * )shm_start_process;
	FILE *file = fopen("sharedmemory.txt", "w");
	for (int i = 0; i < MT->segmentsize + sizeof(struct managementTable); i++)
	{
		fprintf(file ,"%c", shrseg[i]);
	}
	fclose(file);
	*/

	for (int i = 0; i < MAX_PROCESSES; i++)
	{
		if (MT->processes[i].pid == pid)
		{
			MT->processes[i].pid = 0;
			MT->processes[i].shm_start = NULL;
			shm_start_process = NULL;
			sem_post(sem_mutex);
			return 0;
		}
	}

	shm_start_process = NULL;

	sem_post(sem_mutex);
	close_semaphore();
    return -1;
}