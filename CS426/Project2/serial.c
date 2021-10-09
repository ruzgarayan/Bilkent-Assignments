#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>

#define PI 3.14159
#define RADIUS 1.50

typedef enum {false = 0, true = 1} bool;

float randomFloat(float a) {
	return ((float) rand() / RAND_MAX) * a;
}

void randomCirclePoint(float radius, float *_x, float *_y) {
	float x, y;
	float xDiff, yDiff;
	do {
		x = randomFloat(radius * 2);
		y = randomFloat(radius * 2);
		
		xDiff = x - radius;
		yDiff = y - radius;
		
	} while (xDiff * xDiff + yDiff * yDiff > radius * radius);
	
	(*_x) = x;
	(*_y) = y;
}

bool isInRectangle(float x, float y) {
	bool correctY = ((y >= 1) && (y <= 2)) ? true : false;
	bool correctX = ((x - 1.50) * (x - 1.50) <= 2) ? true : false;
	
	return (correctY == true && correctX == true) ? true : false;
}

int main(int argc, char **argv) 
{ 
	if (argc != 2) {
		printf("Enter the number of experiments as an argument.\n\tCorrect usage: ./main-serial 1000\n");
		return 0;
	}

	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	
	srand(time(NULL));
	int numExperiments = atoi(argv[1]);
	float *randomPointRadius, *randomPointPacked, *pointsInRectangle;
	
	float numHits = 0;
	float x, y;
	for (int i = 0; i < numExperiments; i++) {
		randomCirclePoint(RADIUS, &x, &y);
		numHits += isInRectangle(x, y);
	}
	
	float estimation = (numHits / numExperiments) * PI * 9 / 8;
	
	gettimeofday(&end, NULL);
	double elapsedTime = (end.tv_sec - start.tv_sec) +
	(end.tv_usec - start.tv_usec) / 1000000.0;
	
	printf("Estimation of sqrt 2 is: %f\n", estimation);
	printf("Elapsed time: %.10f ms\n", elapsedTime * 1000);
}

