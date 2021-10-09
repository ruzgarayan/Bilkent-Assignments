#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "simplefs.h"

#define SUPERBLOCK_POS 0
#define BITMAP_START 1
#define BITMAP_END 4
#define ROOT_DIRECTORY_START 5
#define ROOT_DIRECTORY_END 8
#define FCB_TABLE_START 9
#define FCB_TABLE_END 12

#define MAX_OPEN_FILES 16
#define FILENAME_LENGTH 110
#define BITMAP_ROWS_PER_BLOCK 1024
#define DIRECTORY_ENTRIES_PER_BLOCK 32
#define FCB_PER_BLOCK 32
#define ROWS_PER_INDEX_TABLE 1024


struct openFileInfo {
    int used; //1 if used, 0 if not used
    int directoryEntryIndex;
    int lastPositionByte;
    int mode;
    pid_t processId;
};

struct superblockData {
    int size;
    struct openFileInfo openFiles[MAX_OPEN_FILES];
};

struct bitmapBlock {
    int bitmap[BITMAP_ROWS_PER_BLOCK];
};

struct directoryEntry {
    char fileName[FILENAME_LENGTH];
    int index; //Will be -1 if the directoryEntry is not used
    char filler[10];
};

struct rootDirectoryBlock {
    struct directoryEntry directoryEntries[DIRECTORY_ENTRIES_PER_BLOCK];
};

struct FCB {
    int used;
    int size;
    int indexBlock;
    char filler[116];
};

struct FCBTableBlock {
    struct FCB FCBList[FCB_PER_BLOCK];
};

struct indexTable {
    int indexTable[ROWS_PER_INDEX_TABLE];
};


// Global Variables =======================================
int vdisk_fd; // Global virtual disk file descriptor. Global within the library.
              // Will be assigned with the vsfs_mount call.
              // Any function in this file can use this.
              // Applications will not use  this directly. 
// ========================================================


// read block k from disk (virtual disk) into buffer block.
// size of the block is BLOCKSIZE.
// space for block must be allocated outside of this function.
// block numbers start from 0 in the virtual disk. 
int read_block (void *block, int k)
{
    int n;
    int offset;

    offset = k * BLOCKSIZE;
    lseek(vdisk_fd, (off_t) offset, SEEK_SET);
    n = read (vdisk_fd, block, BLOCKSIZE);
    if (n != BLOCKSIZE) {
	printf ("read error\n");
	return -1;
    }
    return (0); 
}

// write block k into the virtual disk. 
int write_block (void *block, int k)
{
    int n;
    int offset;

    offset = k * BLOCKSIZE;
    lseek(vdisk_fd, (off_t) offset, SEEK_SET);
    n = write (vdisk_fd, block, BLOCKSIZE);
    if (n != BLOCKSIZE) {
	printf ("write error\n");
	return (-1);
    }
    return 0; 
}

void intToBinary(int num, char *binary) { 
    binary[32] = '\0';   
    for (int i = 31; i >= 0; i--) {
        binary[i] = (num & 1) + '0';
        num >>= 1;
    }
}


/**********************************************************************
   The following functions are to be called by applications directly. 
***********************************************************************/

void bitmap_set_used(int block) {
    //Which of the bitmap blocks 1,2,3,4 is the corresponding bit at
    int bitmapBlock = block / (BLOCKSIZE * 8) + BITMAP_START;
    int rem = block % (BLOCKSIZE * 8);

    //Which of the rows of the current bitmap block the corresponding bit at
    int row = rem / 32;
    int bit = rem % 32;
    //printf("block = %d, row = %d, bit = %d\n", bitmapBlock, row, bit);

    char readBuffer[BLOCKSIZE];
    read_block(readBuffer, bitmapBlock);
    struct bitmapBlock *bitmapBlockData = (struct bitmapBlock *) readBuffer;
    int rowVal = bitmapBlockData->bitmap[row];
    rowVal |= 1UL << (32 - bit - 1); 
    bitmapBlockData->bitmap[row] = rowVal;
    write_block(readBuffer, bitmapBlock);
}

void bitmap_set_free(int block) {
    //Which of the bitmap blocks 1,2,3,4 is the corresponding bit at
    int bitmapBlock = block / (BLOCKSIZE * 8) + BITMAP_START;
    int rem = block % (BLOCKSIZE * 8);

    //Which of the rows of the current bitmap block the corresponding bit at
    int row = rem / 32;
    int bit = rem % 32;

    char readBuffer[BLOCKSIZE];
    read_block(readBuffer, bitmapBlock);
    struct bitmapBlock *bitmapBlockData = (struct bitmapBlock *) readBuffer;
    int rowVal = bitmapBlockData->bitmap[row];
    rowVal &= ~(1UL << (32 - bit - 1)); 
    bitmapBlockData->bitmap[row] = rowVal;
    write_block(readBuffer, bitmapBlock);
}

// this function is partially implemented.
int create_format_vdisk (char *vdiskname, unsigned int m)
{
    char command[1000];
    int size;
    int num = 1;
    int count;
    size  = num << m;
    count = size / BLOCKSIZE;
    //    printf ("%d %d", m, size);
    sprintf (command, "dd if=/dev/zero of=%s bs=%d count=%d",
             vdiskname, BLOCKSIZE, count);
    //printf ("executing command = %s\n", command);
    system (command);
    sfs_mount(vdiskname);

    // now write the code to format the disk below.
    // .. your code...

    char writeBuffer[BLOCKSIZE];

    //Write data to superblock
    struct superblockData *sbData = (struct superblockData *) writeBuffer;
    sbData->size = size;
    for (int i = 0; i < MAX_OPEN_FILES; i++)
    {
        sbData->openFiles[i].used = 0; 
        sbData->openFiles[i].directoryEntryIndex = 0;
        sbData->openFiles[i].lastPositionByte = 0;
        sbData->openFiles[i].mode = MODE_READ;
        sbData->openFiles[i].processId = 0;
    }

    write_block(writeBuffer, SUPERBLOCK_POS);


    //Initiliaze bitmap, first 13 bits are 1, rest are 0.
    for (int i = BITMAP_START; i <= BITMAP_END; i++)
    {
        struct bitmapBlock *bitmapBlock = (struct bitmapBlock *) writeBuffer;
        for (int j = 0; j < BITMAP_ROWS_PER_BLOCK; j++)
        {
            bitmapBlock->bitmap[j] = 0;
        }

        if (i == BITMAP_START)
            bitmapBlock->bitmap[0] = -524288; //First 13 bits 1

        write_block(writeBuffer, i);
    }
    int blockCount = size / BLOCKSIZE;
    int bitmapBlockCount = (BITMAP_END - BITMAP_START + 1) * BITMAP_ROWS_PER_BLOCK * 32;
    for (int i = blockCount; i < bitmapBlockCount; i++)
    {
        bitmap_set_used(i);
    }

    //Initiliaze rootDirectoryBlocks, will be empty
    for (int i = ROOT_DIRECTORY_START; i <= ROOT_DIRECTORY_END; i++)
    {
        struct rootDirectoryBlock *RDBlock = (struct rootDirectoryBlock *) writeBuffer;
        for (int j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++)
        {
            RDBlock->directoryEntries[j].fileName[0] = '\0';
            RDBlock->directoryEntries[j].index = -1;
            RDBlock->directoryEntries[j].filler[0] = '\0';
        }

        write_block(writeBuffer, i);
    }

    //Initialize FCB table, will be 
    //For FCB structs, used bit will be all 0 initially.
    for (int i = FCB_TABLE_START; i <= FCB_TABLE_END; i++)
    {
        struct FCBTableBlock *FCBTableBlock = (struct FCBTableBlock *) writeBuffer;
        for (int j = 0; j < FCB_PER_BLOCK; j++)
        {
            FCBTableBlock->FCBList[j].used = 0;
            FCBTableBlock->FCBList[j].size = 0;
            FCBTableBlock->FCBList[j].indexBlock = -1;
            FCBTableBlock->FCBList[j].filler[0] = '\0';
        }

        write_block(writeBuffer, i);
    } 
    sfs_umount(vdiskname);
    
    return (0); 
}

//Returns the index of a nonzero bit for the given num
int findZeroBit(int num)
{
    int res;
    for (int i = 31; i >= 0; i--) {
        res = num & 1;
        if (res == 0)
            return i;
        num >>= 1;
    }
    return -1;
}

int findFreeBlock() {
    char readBuffer[BLOCKSIZE];
    int zeroBit;

    for (int i = BITMAP_START; i <= BITMAP_END; i++)
    {
        read_block(readBuffer, i);
        struct bitmapBlock *bitmapBlock = (struct bitmapBlock *) readBuffer;

        for (int j = 0; j < BITMAP_ROWS_PER_BLOCK; j++)
        {
            if (bitmapBlock->bitmap[j] == -1) //All bits are 1, so no usable bit
                continue;

            zeroBit = findZeroBit(bitmapBlock->bitmap[j]);
            int freeBlock = (i - BITMAP_START) * BLOCKSIZE * 8 + j * 32 + zeroBit;
            return freeBlock;
        }
    }
    return -1;
}

// already implemented
int sfs_mount (char *vdiskname)
{
    // simply open the Linux file vdiskname and in this
    // way make it ready to be used for other operations.
    // vdisk_fd is global; hence other function can use it. 
    vdisk_fd = open(vdiskname, O_RDWR); 
    return(0);
}


// already implemented
int sfs_umount ()
{
    fsync (vdisk_fd); // copy everything in memory to disk
    close (vdisk_fd);
    return (0); 
}


int sfs_create(char *filename)
{
    //Find empty spot at FCB table (used = 0), let its index be x
    //Also find empty spot at Root directory,
    //Register the file name into directory entry
    //Set the index of the directory entry to x
    //Fill in the FCB details and write them
    int FCBBlockLoc = -1;
    int FCBLoc = -1;
    int rootDirBlockLoc = -1;
    int rootDirLoc = -1;
    char rootReadBuffer[BLOCKSIZE];
    char fcbReadBuffer[BLOCKSIZE];

    //Check if the same file name already exists
    struct rootDirectoryBlock* rootDir;
    for(int i = ROOT_DIRECTORY_START; i <= ROOT_DIRECTORY_END; i++){
        read_block(rootReadBuffer, i);
        rootDir = (struct rootDirectoryBlock*) rootReadBuffer;
        for(int j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++){
            if(rootDir->directoryEntries[j].index > -1 &&
             strcmp(rootDir->directoryEntries[j].fileName, filename) == 0){
                return -1;
            }
        }
    }

    for(int i = ROOT_DIRECTORY_START; i <= ROOT_DIRECTORY_END; i++){
        read_block(rootReadBuffer, i);
        rootDir = (struct rootDirectoryBlock*) rootReadBuffer;
        for(int j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++){
            if(rootDir->directoryEntries[j].index == -1){
                rootDirLoc = j;
                rootDirBlockLoc = i;
                break;
            }
        }
        if(rootDirLoc != -1 && rootDirBlockLoc != -1){
            break;
        }
    }

    if(rootDirLoc == -1 || rootDirBlockLoc == -1){
        return -1;
    }

    struct FCBTableBlock* fcbTable;
    for(int i = FCB_TABLE_START; i <= FCB_TABLE_END; i++){
        read_block(fcbReadBuffer, i);
        fcbTable = (struct FCBTableBlock*) fcbReadBuffer;   
        for(int j = 0; j < FCB_PER_BLOCK; j++){
            if(fcbTable->FCBList[j].used == 0){
                FCBBlockLoc = i;
                FCBLoc = j;
                break; 
            }
        }
        if(FCBBlockLoc != -1 && FCBLoc != -1){
            break;
        }
    }

    if(FCBBlockLoc == -1 || FCBLoc == -1){
        return -1;
    }

    strcpy(rootDir->directoryEntries[rootDirLoc].fileName, filename);
    rootDir->directoryEntries[rootDirLoc].index = (FCBBlockLoc - FCB_TABLE_START) * FCB_PER_BLOCK + FCBLoc;
    
    int indexBlock = findFreeBlock();
    if (indexBlock == -1)
        return -1;

    //printf("Found free block %d for index table\n", indexBlock);
    bitmap_set_used(indexBlock);

    char writeBuffer[BLOCKSIZE];
    struct indexTable *indexTable = (struct indexTable *) writeBuffer;
    for (int i = 0; i < ROWS_PER_INDEX_TABLE; i++)
        indexTable->indexTable[i] = -1;
    write_block(indexTable, indexBlock);

    fcbTable->FCBList[FCBLoc].used = 1;
    fcbTable->FCBList[FCBLoc].size = 0;
    fcbTable->FCBList[FCBLoc].indexBlock = indexBlock;

    write_block(rootDir, rootDirBlockLoc);
    write_block(fcbTable, FCBBlockLoc);
    return (0);
}

int sfs_open(char *file, int mode)
{
    //Search directory entries until matching filename
    //Check if there are empty slots in openFiles array

    //If there is an empty slot, fill it with new information
    //return the index of that slot

    int firstOpenFile = -1;
    struct superblockData* superblock;
    char superReadBuffer[BLOCKSIZE];
    read_block(superReadBuffer, SUPERBLOCK_POS);
    superblock = (struct superblockData*) superReadBuffer;

    for(int i = 0; i < MAX_OPEN_FILES; i++){
        if(superblock->openFiles[i].used == 0){
            firstOpenFile = i;
            break;
        }
    }

    if(firstOpenFile == -1){
        return -1;
    }

    char rootReadBuffer[BLOCKSIZE];
    int directoryEntryIndex = -1;

    struct rootDirectoryBlock* rootDir; 
    for(int i = ROOT_DIRECTORY_START; i <= ROOT_DIRECTORY_END; i++){
        read_block(rootReadBuffer, i);
        rootDir = (struct rootDirectoryBlock*) rootReadBuffer;
        for(int j = 0; j <= DIRECTORY_ENTRIES_PER_BLOCK; j++){
            if(!strcmp(rootDir->directoryEntries[j].fileName, file)){
                directoryEntryIndex = (i - ROOT_DIRECTORY_START) * DIRECTORY_ENTRIES_PER_BLOCK + j;
                break;
            }
        }
        if(directoryEntryIndex != -1){
            break;
        }
    }

    if(directoryEntryIndex == -1){
        return -1;
    }

    superblock->openFiles[firstOpenFile].used = 1;
    superblock->openFiles[firstOpenFile].directoryEntryIndex = directoryEntryIndex;
    superblock->openFiles[firstOpenFile].lastPositionByte = 0;
    superblock->openFiles[firstOpenFile].mode = mode;
    superblock->openFiles[firstOpenFile].processId = getpid();
    write_block(superblock, SUPERBLOCK_POS);

    return (firstOpenFile); 
}

int sfs_close(int fd){
    //Set used field of the entry to 0

    struct superblockData* superblock;
    char superReadBuffer[BLOCKSIZE];
    read_block(superReadBuffer, SUPERBLOCK_POS);
    superblock = (struct superblockData*) superReadBuffer;

    superblock->openFiles[fd].used = 0;
    write_block(superblock, SUPERBLOCK_POS);
    return (0); 
}

int getFCB(int fd, struct FCB* FCB) {
    if(fd >= 16 || fd < 0){
        return -1;
    }
    struct superblockData* superblock;
    char readBuffer[BLOCKSIZE];
    read_block(readBuffer, SUPERBLOCK_POS);
    superblock = (struct superblockData*) readBuffer;

    if(!superblock->openFiles[fd].used)
    {
        return -1;
    }

    int index = superblock->openFiles[fd].directoryEntryIndex;

    struct rootDirectoryBlock* rootDir;
    read_block(readBuffer, ROOT_DIRECTORY_START + (index / DIRECTORY_ENTRIES_PER_BLOCK));
    rootDir = (struct rootDirectoryBlock*) readBuffer;

    int FCBIndex = rootDir->directoryEntries[index % DIRECTORY_ENTRIES_PER_BLOCK].index;
    struct FCBTableBlock* fcbTable;
    read_block(readBuffer, FCB_TABLE_START + (FCBIndex / FCB_PER_BLOCK));
    fcbTable = (struct FCBTableBlock*) readBuffer;

    struct FCB FCB_ = fcbTable->FCBList[FCBIndex % FCB_PER_BLOCK];

    FCB->used = FCB_.used;
    FCB->size = FCB_.size;
    FCB->indexBlock = FCB_.indexBlock;

    return FCBIndex; 
}


int sfs_getsize (int  fd)
{
    char buffer[128];
    struct FCB *FCB = (struct FCB *) buffer;

    getFCB(fd, FCB);

    return FCB->size;
}

int read_from_block(int block, void *buf, int startingByte, int n) {
    //Write into the first n bytes of the buf,
    //the contents of the given block starting from the startingByte
    char readBuffer[BLOCKSIZE];
    read_block(readBuffer, block);

    char *readBufferShifted = readBuffer + startingByte;
    memcpy(buf, readBufferShifted, n);
    return 0;
}

int findBlockOfByte(struct indexTable* indexTable, int byte) {
    return 0;
}

int sfs_read(int fd, void *buf, int n){
    struct superblockData* superblock;
    char readBuffer[BLOCKSIZE];
    read_block(readBuffer, SUPERBLOCK_POS);
    superblock = (struct superblockData*) readBuffer;

    if(!superblock->openFiles[fd].used || (superblock->openFiles[fd].mode != MODE_READ))
    {
        return -1;
    }
    int lastPositionByte = superblock->openFiles[fd].lastPositionByte;
    //printf("Last position byte was %d in the beginning of this read.\n", lastPositionByte);

    char FCBBuffer[128];
    struct FCB *FCB = (struct FCB *) FCBBuffer;
    int FCBIndex = getFCB(fd, FCB);
    if (FCBIndex == -1) return -1;
    if (FCB->indexBlock == -1 || FCB->size == 0) return -1;

    int size = FCB->size;
    int indexBlock = FCB->indexBlock;
    struct indexTable* indexTable;
    char readBuffer2[BLOCKSIZE];
    read_block(readBuffer2, indexBlock);
    indexTable = (struct indexTable*) readBuffer2;

    int remainingBytes = n;
    int startingByte = 0;
    int blockToRead = 0;
    int readAmount = 0;
    int totalRead = 0;
    while (remainingBytes > 0 && lastPositionByte < size)
    {
        blockToRead = indexTable->indexTable[lastPositionByte / BLOCKSIZE];
        if (blockToRead == -1)
            break;

        startingByte = lastPositionByte % BLOCKSIZE;
        readAmount = BLOCKSIZE - startingByte;
        if (readAmount > remainingBytes)
            readAmount = remainingBytes;
        if (lastPositionByte + readAmount > size)
            readAmount = size - lastPositionByte;
        
        //printf("Reading %d bytes of the remaining %d bytes from the block %d\n", 
        //    readAmount, remainingBytes, blockToRead);

        read_from_block(blockToRead, buf, startingByte, readAmount);
        buf = (void*)((char*)buf + readAmount);

        remainingBytes -= readAmount;
        lastPositionByte += readAmount;
        totalRead += readAmount;
    }

    if (totalRead == 0)
        return -1;

    //printf("Last position byte was %d in the end of this read.\n", lastPositionByte);
    superblock->openFiles[fd].lastPositionByte = lastPositionByte;
    write_block(readBuffer, SUPERBLOCK_POS);

    return totalRead; 
}

int allocate_new_block(int FCBIndex) {
    //Allocate a new block for the file, put it to the end of the index table
    //return the allocated block's index.
    char fcbReadBuffer[BLOCKSIZE];
    struct FCBTableBlock* fcbTable;
    read_block(fcbReadBuffer, FCB_TABLE_START + (FCBIndex / FCB_PER_BLOCK));
    fcbTable = (struct FCBTableBlock*) fcbReadBuffer;
    int indexLoc = fcbTable->FCBList[FCBIndex % FCB_PER_BLOCK].indexBlock;

    char indexReadBuffer[BLOCKSIZE];
    struct indexTable* index;
    read_block(indexReadBuffer, indexLoc);
    index = (struct indexTable*) indexReadBuffer;

    int openIndex = -1;
    for(int i = 0; i < ROWS_PER_INDEX_TABLE; i++){
        if(index->indexTable[i] == -1) {
            openIndex = i;
            break;
        }
    } 

    if(openIndex == -1){
        return -1;
    }
    int freeBlockLoc = findFreeBlock();
    bitmap_set_used(freeBlockLoc);
    index->indexTable[openIndex] = freeBlockLoc;
    write_block(index, indexLoc);
    //IF no space in index table, or no available block, return -1
    return freeBlockLoc;
}

int append_to_block(int block, void *buf, int startingByte, int n) {
    //Append first n bytes of the buf to the given block, starting from the startingByte
    char readBuffer[BLOCKSIZE];
    read_block(readBuffer, block);

    char *readBufferShifted = readBuffer + startingByte;
    memcpy(readBufferShifted, buf, n);

    write_block(readBuffer, block);
    return 0;
}

int last_block(int indexTableBlock) {
    char readBuffer[BLOCKSIZE];
    struct indexTable* index;
    read_block(readBuffer, indexTableBlock);
    index = (struct indexTable*) readBuffer;
    
    for(int i = 0; i < ROWS_PER_INDEX_TABLE; i++){
        if(index->indexTable[i] == -1 && i != 0){
            return index->indexTable[i - 1];
        }
    } 
    return -1;
}

int update_FCB(int fd, struct FCB *newFCB) {
    char buffer[128];
    struct FCB *FCB = (struct FCB *) buffer;

    int FCBIndex = getFCB(fd, FCB);
    if (FCBIndex == -1)
        return -1;

    int FCBBlock = FCBIndex / FCB_PER_BLOCK + FCB_TABLE_START;
    char readBuffer[BLOCKSIZE];
    read_block(readBuffer, FCBBlock);
    struct FCBTableBlock *FCBTableBlock = (struct FCBTableBlock *) readBuffer;

    int relativeFCBIndex = FCBIndex % FCB_PER_BLOCK;

    FCBTableBlock->FCBList[relativeFCBIndex].used = newFCB->used;     
    FCBTableBlock->FCBList[relativeFCBIndex].size = newFCB->size; 
    FCBTableBlock->FCBList[relativeFCBIndex].indexBlock = newFCB->indexBlock;

    write_block(readBuffer, FCBBlock);

    return 0;
}

int sfs_append(int fd, void *buf, int n)
{
    char buffer[128];
    struct FCB *FCB = (struct FCB *) buffer;

    int FCBIndex = getFCB(fd, FCB);
    int size = FCB->size;
    int startingByte = size % BLOCKSIZE;
    int block = last_block(FCB->indexBlock);
    int remainingBytes = n;

    while (remainingBytes > 0)
    {
        //printf("Append with remaining %d bytes\n", remainingBytes);

        //Will start to append on a new block, so create it
        if (startingByte == 0)
        {
            block = allocate_new_block(FCBIndex);
            //printf("Allocated block %d\n", block);
            if (block == -1)
                break;
        }

        int remainingSpace = BLOCKSIZE - startingByte;
        //printf("Remaining space on curent block is %d bytes\n", remainingSpace);
        if (remainingBytes > remainingSpace)
        {
            append_to_block(block, buf, startingByte, remainingSpace);
            buf = (void*)((char*)buf + remainingSpace);
            remainingBytes -= remainingSpace;
        } else {
            append_to_block(block, buf, startingByte, remainingBytes);
            remainingBytes = 0;
        }
        startingByte = 0;
    }

    int writtenBytes = n - remainingBytes;
    int newSize = size + writtenBytes;
    FCB->size = newSize;

    update_FCB(fd, FCB);

    return (writtenBytes); 
}


int sfs_delete(char *filename)
{
    //Find Index block, for all entries that are not -1, free those block from the bitmap
    //use bitmap_set_free
    //set used 0 for FCB and set index -1 for directoryEntry

    char rootReadBuffer[BLOCKSIZE];
    struct rootDirectoryBlock* rootDir;
    int FCBLoc = -1;
    int rootDirBlockLoc = -1;
    for(int i = ROOT_DIRECTORY_START; i <= ROOT_DIRECTORY_END; i++){
        read_block(rootReadBuffer, i);
        rootDir = (struct rootDirectoryBlock*) rootReadBuffer;
        for(int j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++){
            if(strcmp(rootDir->directoryEntries[j].fileName, filename) == 0){
                FCBLoc = rootDir->directoryEntries[j].index;
                rootDir->directoryEntries[j].index = -1;
                rootDirBlockLoc = i;
                break;
            }
        }
        if(FCBLoc != -1 && rootDirBlockLoc != -1){
            break;
        }
    }

    if(FCBLoc == -1){
        return -1;
    } 

    write_block(rootDir, rootDirBlockLoc);

    char fcbReadBuffer[BLOCKSIZE];
    struct FCBTableBlock* fcbBlock;
    read_block(fcbReadBuffer, FCBLoc / FCB_PER_BLOCK);
    fcbBlock = (struct FCBTableBlock*) fcbReadBuffer;
    int indexBlockLoc = fcbBlock->FCBList[FCBLoc % FCB_PER_BLOCK].indexBlock;
    fcbBlock->FCBList[FCBLoc % FCB_PER_BLOCK].used = 0;
    fcbBlock->FCBList[FCBLoc % FCB_PER_BLOCK].indexBlock = -1;

    write_block(fcbBlock, FCBLoc / FCB_PER_BLOCK);

    char indexReadBuffer[BLOCKSIZE];
    struct indexTable* index;
    read_block(indexReadBuffer, indexBlockLoc);
    index = (struct indexTable*) indexReadBuffer;
    for(int i = 0; i < ROWS_PER_INDEX_TABLE; i++){
        if(index->indexTable[i] != -1){
            bitmap_set_free(index->indexTable[i]);
        }
    }
    bitmap_set_free(indexBlockLoc);
    return (0); 
}





//This method was used for testing
void printVirtualDiskInfo() {
    char readBuffer[BLOCKSIZE];

    printf("SUPERBLOCK\n___________________________\n");
    read_block(readBuffer, SUPERBLOCK_POS);
    struct superblockData *sbData = (struct superblockData*) readBuffer;

    printf("Size=%d\n", sbData->size);
    for (int i = 0; i < MAX_OPEN_FILES; i++)
    {
        if (sbData->openFiles[i].used)
        {
            printf("openFiles[%d] is used\n", i);
            printf("\tdirectoryEntryIndex = %d, lastPositionByte = %d, mode = %d, processId = %d\n", 
                sbData->openFiles[i].directoryEntryIndex, sbData->openFiles[i].lastPositionByte,
                sbData->openFiles[i].mode, sbData->openFiles[i].processId);
        }
    }
    printf("___________________________\n\n");


    printf("BITMAP (ONLY BLOCK1)\n___________________________\n");
    read_block(readBuffer, BITMAP_START);
    char binary[33];
    struct bitmapBlock *bitmapBlock = (struct bitmapBlock*) readBuffer;
    for (int i = 0; i < BITMAP_ROWS_PER_BLOCK; i++)
    {
        if (i % 4 == 0) printf("\n");
        intToBinary(bitmapBlock->bitmap[i], binary);
        printf("%s ", binary);
    }
    printf("\n___________________________\n\n");


    printf("ROOT DIRECTORY (ONLY BLOCK1)\n___________________________\n");
    read_block(readBuffer, ROOT_DIRECTORY_START);
    struct rootDirectoryBlock *rootDirectoryBlock = (struct rootDirectoryBlock*) readBuffer;
    for (int i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; i++)
    {
        if (rootDirectoryBlock->directoryEntries[i].index != -1)
        {
            printf("directoryEntry[%d] is used\n", i);
            printf("\tfileName = %s, index = %d\n", 
                rootDirectoryBlock->directoryEntries[i].fileName,
                rootDirectoryBlock->directoryEntries[i].index);
        }
    }
    printf("___________________________\n\n");


    printf("FCB TABLE (ONLY BLOCK1)\n___________________________\n");
    read_block(readBuffer, FCB_TABLE_START);
    struct FCBTableBlock *FCBTableBlock = (struct FCBTableBlock*) readBuffer;
    for (int i = 0; i < FCB_PER_BLOCK; i++)
    {
        if (FCBTableBlock->FCBList[i].used)
        {
            printf("FCBList[%d] is used\n", i);
            printf("\tsize = %d, indexBlock = %d\n", 
                FCBTableBlock->FCBList[i].size,
                FCBTableBlock->FCBList[i].indexBlock);

            printf("\tIts index block is:\n");

            char readBuffer2[BLOCKSIZE];
            read_block(readBuffer2, FCBTableBlock->FCBList[i].indexBlock);
            struct indexTable *indexTable = (struct indexTable *) readBuffer2;
            for (int i = 0; i < ROWS_PER_INDEX_TABLE; i++)
            {
                if (indexTable->indexTable[i] == -1) break;
                printf("\t\t%d\n", indexTable->indexTable[i]);
            }
        }
    }
    printf("___________________________\n\n");


}

