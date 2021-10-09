#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <mpi.h>

void countChars(int countResults[], char input[], int size) {
	for (int i = 0; i < size; i++) {
		switch (input[i]) {
			case 'A': countResults[0]++; break;
			case 'C': countResults[1]++; break;
			case 'G': countResults[2]++; break;
			case 'T': countResults[3]++; break;
		}
	}   
}

int main(int argc, char **argv) 
{ 
	if (argc != 2) {
		printf("Incorrect usage. Give the file name as an argument. Example:\n\tmpirun -np 2 ./dna-mpi-v2 DNA.txt\n");
		return 0;
	}
	
	MPI_Status s;
	double start, end;
	int numprocs, myid;
    	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	
	char *readLine;
	int results[4];
	for (int i = 0; i < 4; i++) results[i] = 0;
	
	if (myid == 0)
	{
		start = MPI_Wtime();
		FILE* file;
		file = fopen(argv[1], "rb");
		if (file == 0)
			return 0;
			
		size_t len;
		char *readLine = NULL;
		getline(&readLine, &len, file);	
		
		//Send the number of chars per line, to the other processors 	
		for (int i = 1; i < numprocs; i++) {
			MPI_Send ((void *)&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}	
		
		int nextProc = 0;
		do {
			//If it's the turn for this processors, make the computation
			if (nextProc == 0) countChars(results, readLine, len);
			//If it's the turn for other processors, send the current line to them  
			else MPI_Send (readLine, len, MPI_CHAR, nextProc, 1, MPI_COMM_WORLD);
			
			//Advance the turn
			nextProc = (nextProc + 1) % numprocs;
		} while(getline(&readLine, &len, file) != -1);
		
		//Send this to all the processors to indicate that all the lines are read
		readLine[0] = '\0';
		for (int i = 1; i < numprocs; i++) {
			MPI_Send (readLine, len, MPI_CHAR, i, 1, MPI_COMM_WORLD);
		}
		free(readLine);
		
		end = MPI_Wtime();
		printf("Proc %d, Elapsed time: %.10f ms\n", myid, (end - start) * 1000);
	}
	else {
		start = MPI_Wtime();
		int lineLen;
		
		//Receive the length of one line
		MPI_Recv((void *)&lineLen, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &s);
		readLine = malloc(lineLen);
		
		//Receive data from processor 0 and process them until the end signal comes
		MPI_Recv(readLine, lineLen, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &s);
		while(readLine[0] != '\0') {
			countChars(results, readLine, lineLen);
			MPI_Recv(readLine, lineLen, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &s);
		}
		
		MPI_Send (results, 4, MPI_INT, 0, 2, MPI_COMM_WORLD);
	}
	
	//Use Allreduce to sum up all the results
	int resultSums[4];
	MPI_Allreduce(&results, &resultSums, 4, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
              
	end = MPI_Wtime();
	printf("Results in proc %d, C=%d, T=%d, G=%d, A=%d\n", myid, resultSums[1], resultSums[3], resultSums[2], resultSums[0]);
	//printf("Proc %d, Elapsed time: %.10f ms\n", myid, (end - start) * 1000);

	MPI_Finalize(); 
}


//Faster working main
/*
int main(int argc, char **argv) 
{ 
	MPI_Status s;
	double start, end;
	int numprocs, myid;
    	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	
	char *readBuf;
	int results[4];
	for (int i = 0; i < 4; i++) results[i] = 0;
	
	FILE* file;
	file = fopen(argv[1], "rb");
	if (file == 0)
		return 0;
	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	if (myid == 0)
	{
		start = MPI_Wtime();
		
		int sizePerProc = (fsize / numprocs) + 1;
		for (int i = 1; i < numprocs; i++) {
			MPI_Send ((void *)&sizePerProc, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		
		readBuf = malloc(sizePerProc);
		
		for (int i = 1; i < numprocs; i++) {
			fread(readBuf, 1, sizePerProc, file);
			MPI_Send (readBuf, sizePerProc, MPI_CHAR, i, 1, MPI_COMM_WORLD);
			fsize -= sizePerProc;
		}
		
		fread(readBuf, 1, fsize, file);
		countChars(results, readBuf, fsize);
		
	}
	else {
		start = MPI_Wtime();
		int a = 0, c = 0, g = 0, t = 0;
		int size;
		
		MPI_Recv((void *)&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &s);
		readBuf = malloc(size);
		MPI_Recv(readBuf, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &s);
		countChars(results, readBuf, size);
		
	}
	
	int resultSums[4];
	MPI_Allreduce(&results, &resultSums, 4, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
              
	end = MPI_Wtime();
	printf("Results in proc %d, A=%d, C=%d, G=%d, T=%d\n", myid, resultSums[0], resultSums[1], resultSums[2], resultSums[3]);
	printf("Proc %d, Elapsed time: %.10f seconds\n", myid, end - start);

	MPI_Finalize(); 
}
*/
