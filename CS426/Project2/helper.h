#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <mpi.h>

typedef enum {false = 0, true = 1} bool;

float *MPI_Map_Func(float *arr, int size, float (*func)(float)) {
	int numprocs, myid;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	
	int sizePerProc = size / numprocs;
    	float *localArr = malloc(sizePerProc * sizeof(float));
    	
    	//Divide the arr into all the processes
	MPI_Scatter(arr, sizePerProc, MPI_FLOAT, localArr, sizePerProc, MPI_FLOAT, 0 , MPI_COMM_WORLD);
	
	//Every process makes their computation here.
	for (int i = 0; i < sizePerProc; i++) {
		localArr[i] = func(localArr[i]);
	}
	
	float *results;
	if (myid == 0) results = malloc(size * sizeof(float));
	
	//Gather all the results back in the process 0.
	MPI_Gather(localArr, sizePerProc, MPI_FLOAT, results, sizePerProc, MPI_FLOAT, 0, MPI_COMM_WORLD);
	
	//Some of the last elements of arr are not scattered to any proc if size is not divisible by numproc
	//Do these manually on the process 0. (There are at most numproc - 1 such elements, this will not increase the load on process 0)
	if (myid == 0) {
		for (int i = sizePerProc * numprocs; i < size; i++) {
			results[i] = func(arr[i]);
		}
	}
	
	free(localArr);
	return results;
}

float MPI_Fold_Func(float *arr, int size, float initial_value, float (*func)(float, float)) {
	int numprocs, myid;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	
	int sizePerProc = size / numprocs;
    	float *localArr = malloc(sizePerProc * sizeof(float));
    	
    	//Divide the arr into all the processes
	MPI_Scatter(arr, sizePerProc, MPI_FLOAT, localArr, sizePerProc, MPI_FLOAT, 0 , MPI_COMM_WORLD);
	
	float localResult = initial_value;
	float *localResults = NULL;
	
	//Every process makes their computation here.
	for (int i = 0; i < sizePerProc; i++) 
		localResult = func(localResult, localArr[i]);
	
	if (myid == 0) localResults = malloc(numprocs * sizeof(float));
	
	//Gather all the results back in the process 0.
	MPI_Gather(&localResult, 1, MPI_FLOAT, localResults, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	
	float result = initial_value;	
	if (myid == 0) {
		//Combine the local results;
		for (int i = 0; i < numprocs; i++) 
			result = func(result, localResults[i]);
	
		//Some of the last elements of arr are not scattered to any proc if size is not divisible by numproc
		//Do these manually on the proc 0. (There are at most numproc - 1 such elements, this will not increase the load on proc 0)
		for (int i = sizePerProc * numprocs; i < size; i++) {
			result = func(result, arr[i]);
		}
		
		free(localResults);
	}
	
	free(localArr);
	return result;
}

float *MPI_Filter_Func(float *arr, int size, bool (*func)(float)) {
	int numprocs, myid;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	
	int sizePerProc = size / numprocs;
    	float *localArr = malloc(sizePerProc * sizeof(float));
    	
    	//Divide the arr into all the processes
	MPI_Scatter(arr, sizePerProc, MPI_FLOAT, localArr, sizePerProc, MPI_FLOAT, 0 , MPI_COMM_WORLD);
	
	//Every process makes their computation here.
	for (int i = 0; i < sizePerProc; i++) {
		localArr[i] = func(localArr[i]);
	}
	
	float *results;
	if (myid == 0) results = malloc(size * sizeof(float));
	
	//Gather all the results back in the process 0.
	MPI_Gather(localArr, sizePerProc, MPI_FLOAT, results, sizePerProc, MPI_FLOAT, 0, MPI_COMM_WORLD);
	
	//Some of the last elements of arr are not scattered to any proc if size is not divisible by numproc
	//Do these manually on the proc 0. (There are at most numproc - 1 such elements, this will not increase the load on proc 0)
	if (myid == 0) {
		for (int i = sizePerProc * numprocs; i < size; i++) {
			results[i] = func(arr[i]);
		}
	}
	
	free(localArr);
	return results;
}

