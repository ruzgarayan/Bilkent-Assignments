#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <time.h>
#include <math.h>
#include <omp.h>

//Check if row1 and row2 are independent
int checkIndependentRows(int **matrix, int n, int row1, int row2) {
	int row1First = matrix[row1][0];
	int row2First = matrix[row2][0];

	int isIndependent = 0;
	
	#pragma omp parallel for shared(matrix) reduction(||: isIndependent)
	for (int i = 0; i < n; i++) {
		if (matrix[row1][i] * row2First != matrix[row2][i] * row1First){
			isIndependent = 1;
		}
	}
	
	return isIndependent;
}

//Two other parallelizations that do not necessarily iterate over all the elements of rows
/*

//Implement omp parallel for manually to be able to stop the for loop
int checkIndependentRowsAlternative(int **matrix, int n, int row1, int row2) {
	int row1First = matrix[row1][0];
	int row2First = matrix[row2][0];

	int isIndependent = 0;
	
	#pragma omp parallel shared(matrix, isIndependent)
	{
		int numThreads = omp_get_num_threads();
		int myid = omp_get_thread_num();
		int elementPerThread = n / numThreads;
	
		int start = myid * elementPerThread;
		int end = (myid == numThreads - 1) ? n : ((myid + 1) * elementPerThread);
	
		for (int i = start; i < end && !isIndependent; i++) {
			if (matrix[row1][i] * row2First != matrix[row2][i] * row1First){
				isIndependent = 1;
			}
		}
	}
	
	return isIndependent;
}

//Uses "omp cancel" to do what break does. Requires setting the environment variable OMP_CANCELLATION=true
//Still very slow.
int checkIndependentRowsAlternative2(int **matrix, int n, int row1, int row2) {
	int row1First = matrix[row1][0];
	int row2First = matrix[row2][0];

	int isIndependent = 0;
	
	#pragma omp parallel for shared(matrix) reduction(||: isIndependent)
	for (int i = 0; i < n; i++) {
		if (matrix[row1][i] * row2First != matrix[row2][i] * row1First){
			isIndependent = 1;
			#pragma omp cancel for
		}
	}
	
	return isIndependent;
}

*/

int main(int argc, char **argv) 
{ 
	double startTime, endTime;

	int **matrix;
	int *isRowIndependent;
	int n = atoi(argv[1]);
	
	srand(time(NULL));
	
	//Allocate the memory
	matrix = malloc(n * sizeof(int *));
	isRowIndependent = malloc(n * sizeof(int));
	for (int i = 0; i < n; i++) {
		matrix[i] = malloc(n * sizeof(int));
	}
	
	//Generate random matrix
	for (int i = 0; i < n; i++) {
		isRowIndependent[i] = 1; //All rows are initially declared as independent
		for (int j = 0; j < n; j++) {
			matrix[i][j] = rand() % 10;
		}
	}
	
	startTime = omp_get_wtime();

	//Iterate over all the rows
	for (int i = 0; i < n; i++) {

		//If the current row is not independent, we don't need to check it with others
		if (!isRowIndependent[i]) continue;

		//If the current row is independent, check it with the lower rows
		for (int j = i + 1; j < n; j++) {
			if (!isRowIndependent[j]) continue;
			int isIndependent = checkIndependentRows(matrix, n, i, j);
			
			//Eliminate dependent rows, they will not be checked in further iterations
			if (!isIndependent) {
				isRowIndependent[j] = 0;
			}
		}
	}
	
	int rank = 0;
	for (int i = 0; i < n; i++) {
		rank += isRowIndependent[i];
	}
	
	endTime = omp_get_wtime();
	
	printf("Rank is %d\n", rank);
	printf("Execution time: %.10f seconds.\n", endTime - startTime);
	
	for (int i = 0; i < n; i++) {
		free(matrix[i]);
	}
	free(matrix);
	free(isRowIndependent);
	
	return 0;
}

