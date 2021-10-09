#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char **argv) 
{ 
	if (argc != 2) {
		printf("Incorrect usage. Give the file name as an argument. Example:\n\t./dna-serial DNA.txt\n");
		return 0;
	}

	int a = 0, c = 0, g = 0, t = 0;
	
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	FILE* file;
	file = fopen(argv[1], "r");
	if (file == 0)
		return 0;
	
	size_t len = 0;
	char *readLine = NULL;
	getline(&readLine, &len, file);
	if (readLine == NULL) return 0;
	
	do {
		for (int i = 0; i < len; i++) {
			switch (readLine[i]) {
				case 'A': a++; break;
				case 'C': c++; break;
				case 'G': g++; break;
				case 'T': t++; break;
			}
		}   
	} while(getline(&readLine, &len, file) != -1);
	
	free(readLine);
	fclose(file);
        
        printf("C=%d\nT:%d\nG:%d\nA:%d\n", c, t, g, a);
        
	gettimeofday(&end, NULL);
	double elapsedTime = (end.tv_sec - start.tv_sec) +
	(end.tv_usec - start.tv_usec) / 1000000.0;
	printf("Elapsed time: %.10f ms\n", elapsedTime * 1000);
}

//Faster working main
/*
int main(int argc, char **argv) 
{ 
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	FILE* file;
	file = fopen(argv[1], "r");
	if (file == 0)
		return 0;
		
	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);
	printf("fsize = %lu\n", fsize);
	
	char *string = malloc(fsize + 1);
	fread(string, 1, fsize, file);
	fclose(file);
	string[fsize] = 0;

	int a = 0, c = 0, g = 0, t = 0;
	for (int i = 0; i < fsize; i++) {
		switch (string[i]) {
			case 'A': a++; break;
			case 'C': c++; break;
			case 'G': g++; break;
			case 'T': t++; break;
		}
	}   
        
        printf("A=%d, C=%d, G=%d, T=%d\n", a, c, g, t);
        
	gettimeofday(&end, NULL);
	double elapsedTime = (end.tv_sec - start.tv_sec) +
	(end.tv_usec - start.tv_usec) / 1000000.0;
	printf("Elapsed time: %.10f seconds\n", elapsedTime);
}
*/
