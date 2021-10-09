#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <time.h>
#include <math.h>
#include <sys/time.h>

//Check if row1 and row2 are independent
int checkIndependentRows(int **matrix, int n, int row1, int row2) {
	int row1First = matrix[row1][0];
	int row2First = matrix[row2][0];

	int isIndependent = 0;

	for (int i = 0; i < n; i++) {
		if (matrix[row1][i] * row2First != matrix[row2][i] * row1First)
			isIndependent = 1;
	}
	
	return isIndependent;
}

int main(int argc, char **argv) 
{ 
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
	
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	//Iterate over all the rows
	for (int i = 0; i < n; i++) {
	
		//If the current row is not independent, we don't need to check it with others
		if (!isRowIndependent[i]) continue;
		
		//If the current row is independent, check it with the lower rows
		for (int j = i + 1; j < n; j++) {
			if (!isRowIndependent[j]) continue; //Don't need to check it with dependent rows
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
	
	gettimeofday(&end, NULL);
	double elapsedTime = (end.tv_sec - start.tv_sec) +
	(end.tv_usec - start.tv_usec) / 1000000.0;
	
	printf("Rank is %d\n", rank);
	printf("Execution time: %.10f seconds.\n", elapsedTime);
	
	for (int i = 0; i < n; i++) {
		free(matrix[i]);
	}
	free(matrix);
	free(isRowIndependent);
	
	return 0;
}

