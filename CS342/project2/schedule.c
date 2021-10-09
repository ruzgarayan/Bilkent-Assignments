#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <limits.h>


#include "LinkedList.h"

int N, Bcount, minB, avgB, minA, avgA, ALG;
char inprefix[128];

//No synchronization is needed for these two arrays since every 
//thread accesses and modifies only its own index. So there is no race condition.
double vruntimes[10];
int threadExited[10];

//Synchronization for this queue is obtained by the pthread_mutex variable below
List rq;

pthread_mutex_t mutexQueue;
pthread_cond_t condHasBurst;

int generateExponentiallyRandomNum(int avg, int min)
{
	int res = 0;
	while (res < min)
	{
		int randNum = rand();
		double between0and1 = (double)randNum / (double)RAND_MAX;
		//Subtracting from 1 to prevent infinite results
		res = (int)(log(1.0 - between0and1) * (-avg)); 
	}

	return res;
}

void *generateBursts(void *param)
{
	int threadNum = *((int *) param);

	//srand(threadNum); //CAN BE USED FOR REDUCING THE FACTOR OF RANDOMNESS IN THE EXPERIMENTS
	srand(time(NULL) + threadNum);

	for (int i = 1 ; i <= Bcount; i++)
	{
		int interArrivalTime = generateExponentiallyRandomNum(avgA, minA);
		usleep(interArrivalTime * 1000);
		//printf("Thread with num %d returned from sleep of %d ms\n", threadNum, interArrivalTime);
		int cpuBurstTime = generateExponentiallyRandomNum(avgB, minB);

		struct BurstInfo burstInfo;
		burstInfo.threadIndex = threadNum;
		burstInfo.burstIndex = i;
		burstInfo.length = cpuBurstTime;
		gettimeofday(&burstInfo.generationTime, NULL); 

		pthread_mutex_lock(&mutexQueue);

		//Critical section starts
		insertLast(rq, burstInfo);
		pthread_cond_signal(&condHasBurst); //Signal the main thread that a new burst has been added.
		//Critical section ends

		pthread_mutex_unlock(&mutexQueue);
	}

	threadExited[threadNum - 1] = 1; 
	pthread_exit(0);
};

void *generateBurstsFromFile(void *param)
{
	int threadNum = *((int *) param);
	printf("Thread with num %d started\n", threadNum);

	FILE* file;
	char fileName[256];
	sprintf(fileName, "%s-%d.txt", inprefix, threadNum);
	file = fopen(fileName, "r");
	if (file == 0)
	{
		pthread_exit(0);
	}

	char linebuf[100];
	int interArrivalTime, cpuBurstTime;
	int i = 1;
	while (fgets(linebuf, sizeof linebuf, file) != NULL)
	{
		sscanf(linebuf, "%d%d", &interArrivalTime, &cpuBurstTime);

		usleep(interArrivalTime * 1000);

		struct BurstInfo burstInfo;
		burstInfo.threadIndex = threadNum;
		burstInfo.burstIndex = i++;
		burstInfo.length = cpuBurstTime;
		gettimeofday(&burstInfo.generationTime, NULL); 

		pthread_mutex_lock(&mutexQueue);

		//Critical section
		insertLast(rq, burstInfo);
		pthread_cond_signal(&condHasBurst);

		pthread_mutex_unlock(&mutexQueue);
	}

	threadExited[threadNum - 1] = 1; 
	pthread_exit(0);
};

struct BurstInfo scheduleNextBurst()
{
	if (ALG == 0) //FCFS
	{
		return deleteFirst(rq);
	}
	else if (ALG == 1) //SJF
	{
		int checkedThread[10];
		for (int i = 0; i < N; i++) checkedThread[i] = 0;

		int minValue = INT_MAX;
		int minIndex = 0;
		int size = getSize(rq);
		for (int i = 0; i < size; i++)
		{
			struct BurstInfo burstInfo = get(rq, i);

			if (!checkedThread[burstInfo.threadIndex - 1] && burstInfo.length < minValue)
			{
				minIndex = i;
				minValue = burstInfo.length;
				checkedThread[burstInfo.threadIndex - 1] = 1;
			}
		}

		return deleteAtIndex(rq, minIndex);
	}
	else if (ALG == 2) //PRIO
	{
		int checkedThread[10];
		for (int i = 0; i < N; i++) checkedThread[i] = 0;

		int minValue = INT_MAX;
		int minIndex = 0;
		int size = getSize(rq);
		for (int i = 0; i < size; i++)
		{
			struct BurstInfo burstInfo = get(rq, i);

			if (!checkedThread[burstInfo.threadIndex - 1] && burstInfo.threadIndex < minValue)
			{
				minIndex = i;
				minValue = burstInfo.threadIndex;
			}

			checkedThread[burstInfo.threadIndex - 1] = 1;
		}

		return deleteAtIndex(rq, minIndex);
	}
	else if (ALG == 3) //VRUNTIME
	{
		int checkedThread[10];
		for (int i = 0; i < N; i++) checkedThread[i] = 0;

		int minValue = INT_MAX;
		int minIndex = 0;
		int size = getSize(rq);
		for (int i = 0; i < size; i++)
		{
			struct BurstInfo burstInfo = get(rq, i);

			if (!checkedThread[burstInfo.threadIndex - 1] && vruntimes[burstInfo.threadIndex - 1] < minValue)
			{
				minIndex = i;
				minValue = vruntimes[burstInfo.threadIndex - 1];
			}

			checkedThread[burstInfo.threadIndex - 1] = 1;
		}

		struct BurstInfo burstInfo = get(rq, minIndex);
		vruntimes[burstInfo.threadIndex - 1] += (0.7 + 0.3 * burstInfo.threadIndex) * burstInfo.length;
		return deleteAtIndex(rq, minIndex);
	}
	else
	{
		struct BurstInfo emptyBurstInfo;
		emptyBurstInfo.threadIndex = -1;
		emptyBurstInfo.burstIndex = -1;
		emptyBurstInfo.length = -1;
		return emptyBurstInfo;
	}
}

int main(int argc, char *argv[])
{
	pthread_t *tids;
	pthread_attr_t *attrs;
	struct timeval time;

	if (argc != 8 && argc != 5)
	{
		printf ("Incorrect number of arguments. Correct usage is"
		 "\n\tschedule <N> <Bcount> <minB> <avgB> <minA> <avgA> <ALG>"
		 "\n\tOR"
		 "\n\tschedule <N> <ALG> -f <inprefix>\n");
		return 0;
	}

	N = atoi(argv[1]);
	tids = malloc(N * sizeof(pthread_t));
	attrs = malloc(N * sizeof(pthread_attr_t));
	int threadNums[10];
	for (int i = 0; i < N; i++) pthread_attr_init (&attrs[i]);
	for (int i = 0; i < N; i++) threadExited[i] = 0; 
	for (int i = 0; i < N; i++) vruntimes[i] = 0;
	for (int i = 0; i < 10; i++) threadNums[i] = i + 1;

	rq = initList();

	pthread_mutex_init(&mutexQueue, NULL);
	pthread_cond_init(&condHasBurst, NULL);

	if (argc == 8)
	{
		Bcount = atoi(argv[2]);
		minB = atoi(argv[3]);
		avgB = atoi(argv[4]);
		minA = atoi(argv[5]);
		avgA = atoi(argv[6]);

		if (minA < 100 || minB < 100)
		{
			printf("Min values cannot be smaller than 100 ms\n");
			return 0;
		}
		
		if (strcmp(argv[7], "FCFS") == 0) ALG = 0;
		else if (strcmp(argv[7], "SJF") == 0) ALG = 1;
		else if (strcmp(argv[7], "PRIO") == 0) ALG = 2;
		else if (strcmp(argv[7], "VRUNTIME") == 0) ALG = 3;
		else {
			printf("Incorrect algorithm name, use one of the following:\n\t FCFS, SJF, PRIO, VRUNTIME\n");
			return 0;
		}
		
		for (int i = 0; i < N; i++) {
			pthread_create(&tids[i], &attrs[i], generateBursts, (void *)&threadNums[i]); 
		}
	}

	if (argc == 5)
	{
		if (strcmp(argv[3], "-f") != 0) {
			printf ("Incorrect arguments. Correct usage is"
			 "\n\tschedule <N> <Bcount> <minB> <avgB> <minA> <avgA> <ALG>"
			 "\n\tOR"
			 "\n\tschedule <N> <ALG> -f <inprefix>\n");
			return 0;
		}
		strcpy(inprefix, argv[4]);
		for (int i = 0; i < N; i++) {
			pthread_create(&tids[i], &attrs[i], generateBurstsFromFile, (void *)&threadNums[i]); 
		}
	}

	int allThreadsExited = 0;
	
	//Variables for keeping the statistics
	double threadWaitingTime[10];
	for (int i = 0; i < 10; i++) threadWaitingTime[i] = 0;
	int totalBurstTime = 0;
	int numBursts[10];
	for (int i = 0; i < 10; i++) numBursts[i] = 0;

	while (1)
	{
		pthread_mutex_lock(&mutexQueue);

		//Critical section starts
		while(empty(rq))
		{
			allThreadsExited = 1;
			for (int i = 0; i < N; i++)
			{
				if (!threadExited[i])
					allThreadsExited = 0;
			}
			if (allThreadsExited && empty(rq))
				break;

			//Here, thread releases the lock of mutexQueue when waiting.
			pthread_cond_wait(&condHasBurst, &mutexQueue);
		}
		if (allThreadsExited && empty(rq))
				break;

		struct BurstInfo burstInfo;
		burstInfo = scheduleNextBurst(); 
		//Critical section ends

		pthread_mutex_unlock(&mutexQueue);

		gettimeofday(&time, NULL);
		double elapsedTime = (time.tv_sec - burstInfo.generationTime.tv_sec) +
  		(time.tv_usec - burstInfo.generationTime.tv_usec) / 1000000.0;
		printf("Received %dth burst from thread %d, with length %d ms \n", burstInfo.burstIndex, burstInfo.threadIndex, burstInfo.length);
		printf("\tThis burst have been waiting %f seconds in rq\n\n", elapsedTime);

		threadWaitingTime[burstInfo.threadIndex - 1] += elapsedTime;
		totalBurstTime += burstInfo.length;
		numBursts[burstInfo.threadIndex - 1]++;

		usleep(burstInfo.length * 1000);
	}

	double totalWaitingTime = 0;
	int totalNumBursts = 0;
	for (int i = 0; i < N; i++)
	{
		printf("Thread %d's average waiting time = %f seconds\n", i + 1, threadWaitingTime[i] / numBursts[i]);
		totalWaitingTime += threadWaitingTime[i];
		totalNumBursts += numBursts[i];
	}

	printf("Total simulated burst time was %d ms\n", totalBurstTime);
	printf("Average waiting time for all %d threads is %f seconds\n", N, totalWaitingTime / totalNumBursts);

	for (int i = 0; i < N; i++) {
		pthread_join(tids[i], NULL); 
	}

	deleteList(rq);
	pthread_mutex_destroy(&mutexQueue);
	pthread_cond_destroy(&condHasBurst);
	free(tids);
	free(attrs);
}
