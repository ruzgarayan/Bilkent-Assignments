#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <mpi.h>

#define PRICES_SIZE 100

int parseFiles(char* pricesFile, char* weightsFile,
		 double *prices, double **_weights) 
{
	FILE* file;
	char *readLine = NULL;
	size_t len;
	
	//Read the prices file
	file = fopen(pricesFile, "r");
	if (file == 0)
		return 0;
	for (int i = 0; i < PRICES_SIZE; i++) {
		getline(&readLine, &len, file);
		prices[i] = atof(readLine);
	}
	fclose(file);
	free(readLine);
	
	file = fopen(weightsFile, "r");
	if (file == 0)
		return 0;
	
	//Read first line to find out the number of columns
	readLine = NULL;
	getline(&readLine, &len, file);
	int countCommas = 0;
	for (int i = 0; i < len; i++) {
		if (readLine[i] == ',') countCommas++;
	}
	
	int columns = countCommas + 1;
	double *weights = malloc(columns * PRICES_SIZE * sizeof(double));
	
	int cnt = 0;
	for (int i = 0; i < PRICES_SIZE; i++) {
		char *num = strtok(readLine, ", ");
		do { 
			//This way, we read the file into the array such that columns of data is contigious. 
		    	weights[cnt * PRICES_SIZE + i] = atof(num);
		    	cnt = (cnt + 1) % columns;
		} while (num = strtok(NULL, ", "));
		getline(&readLine, &len, file);
	}
	fclose(file);
	free(readLine);
	
	*(_weights) = weights;
	return columns;
}


void writeOutput(double *output, int columns) {
	FILE* file;
	char *readLine = NULL;
	size_t len;
	
	file = fopen("NN-mpi-v1-output.txt", "w");
	if (file == 0)
		return ;
	
	for (int i = 0; i < columns; i++) {
		fprintf(file,"%.2f ", output[i]);
	}
	fprintf(file,"\n");
	fclose(file);
}

int main(int argc, char **argv) 
{
	if (argc != 3) {
		printf("Incorrect usage. Give the file names as arguments. Example:\n\tmpirun -np 11 ./NN-mpi-v1 price.txt Weights.txt\n");
		return 0;
	}
	
	double prices[PRICES_SIZE];
	
	MPI_Status s;
	int numprocs, myid;
	double start, end;
    	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	
	if (numprocs != 11) {
		if (myid == 0) printf("For this program there must be 10 worker and 1 master process.\n You must call with -np 11.\n");
		MPI_Finalize(); 
		return 0;
	}
	
	if (myid == 0)
	{
		double *weights, *output;
		
		int columns = parseFiles(argv[1], argv[2], prices, &weights);
		output = malloc(columns * sizeof(double));
		start = MPI_Wtime();
		
		//In this part, there are always 10 workers
		int numWorkers = 10;
		//Send the price data to all workers
		for (int i = 1; i <= numWorkers; i++)
			MPI_Send (prices, PRICES_SIZE, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
		
		//Find out how many columns will be given to each worker
		int colPerWorker = columns / numWorkers;
		int sendCnt;
		for (int i = 1; i <= numWorkers; i++) {
			//Some workers will be given one extra column
			sendCnt = ((i - 1) < (columns % numWorkers)) ? colPerWorker + 1 : colPerWorker;
			MPI_Send ((void *)&sendCnt, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
			
		//Send the weights data to corresponding workers
		int startInd = 0; 
		for (int i = 1; i <= numWorkers; i++) {
			sendCnt = ((i - 1) < (columns % numWorkers)) ? colPerWorker + 1 : colPerWorker;
			if (sendCnt == 0) continue; //No further communication if that worker is not given any columns
			MPI_Send (&weights[startInd], sendCnt * PRICES_SIZE, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
			startInd += sendCnt * PRICES_SIZE;
		}
		
		//Collect the results
		int curr = 0;
		for (int i = 1; i <= numWorkers; i++) {
			sendCnt = ((i - 1) < (columns % numWorkers)) ? colPerWorker + 1 : colPerWorker;
			if (sendCnt == 0) continue; //No further communication if that worker is not given any columns
			MPI_Recv(&output[curr], sendCnt, MPI_DOUBLE, i, 3, MPI_COMM_WORLD, &s);
			curr += sendCnt;
		}
		
		end = MPI_Wtime();
		printf("Elapsed time: %.10f ms\n", (end - start) * 1000);
			
		writeOutput(output, columns);
		free(weights);
		free(output);
	}
	else {
		int numAssignedCols;
		double *workerWeights, *workerOutput;
		
		//Receive the price data
		MPI_Recv(prices, PRICES_SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &s);
		
		//Receive the number of assigned columns so that required memory can be allocated
		MPI_Recv((void *)&numAssignedCols, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &s);
		
		//Don't need to do anything if no columns are assigned
		if (numAssignedCols > 0) {
			workerWeights = malloc(numAssignedCols * PRICES_SIZE * sizeof(double));
			workerOutput = malloc(numAssignedCols * sizeof(double));
			
			//Receive all the assigned columns at once
			MPI_Recv(workerWeights, numAssignedCols * PRICES_SIZE, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &s);
			
			//Do the computation
			for (int i = 0; i < numAssignedCols; i++) {
				workerOutput[i] = 0;
				for (int j = 0; j < PRICES_SIZE; j++) {
					workerOutput[i] += prices[j] * workerWeights[i * PRICES_SIZE + j];
				}
			}
			
			//Send all the outputs at once, master process knows which column they are
			MPI_Send (workerOutput, numAssignedCols, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
			
			free(workerWeights);
			free(workerOutput);
		}
	}
	
	MPI_Finalize(); 
	return 0;
}
