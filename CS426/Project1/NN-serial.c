#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <sys/time.h>

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
	
	file = fopen("NN-serial-output.txt", "w");
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
		printf("Incorrect usage. Give the file names as arguments. Example:\n\t./NN-serial price.txt Weights.txt\n");
		return 0;
	}

	double prices[PRICES_SIZE];
	double *weights, *output;
	
	struct timeval start, end;
	
	int columns = parseFiles(argv[1], argv[2], prices, &weights);
	output = malloc(columns * sizeof(double));
	
	gettimeofday(&start, NULL);
	
	for (int i = 0; i < columns; i++) {
		output[i] = 0;
		for (int j = 0; j < PRICES_SIZE; j++) {
			output[i] += prices[j] * weights[i * PRICES_SIZE + j];
		}
	}
	
	gettimeofday(&end, NULL);
	double elapsedTime = (end.tv_sec - start.tv_sec) +
	(end.tv_usec - start.tv_usec) / 1000000.0;
	printf("Elapsed time: %.10f ms\n", elapsedTime * 1000);
	
	writeOutput(output, columns);
	free(weights);
	free(output);
	
	return 0;
}
