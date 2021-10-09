#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <time.h>
#include <mpi.h>
#include "helper.h"
#include <math.h>

#define PI 3.14159
#define RADIUS 1.50

/*
The following two functions, pack_float and unpack_float were taken from the discussion at
https://stackoverflow.com/questions/20752344/storing-2-single-floats-in-one
*/

double scaleFactor = 16384.0;
double cp = 256.0 * 256.0;
/* packs given two floats into one float */
float pack_float(float x, float y) {
    int x1 = (int) (x * scaleFactor);
    int y1 = (int) (y * scaleFactor);
    float f = (y1 * cp) + x1;
    return f;
}
/* unpacks given float to two floats */
void unpack_float(float f, float* x, float* y){
  double dy = floor(f / cp);
  double dx = f - (dy * cp);
  *y = (float) (dy / scaleFactor);
  *x = (float) (dx / scaleFactor);
}

//Helper function to generatea a random float in range [0,a]
float randomFloat(float a) {
	return ((float) rand() / RAND_MAX) * a;
}

/*
Generates a point in the circle with given radius and center at (radius,radius).
Used in the MPI_Map_Func to generate random points in parallel.
AFter generating the x and y coordinates of the point, packs and returns them as a single float.
*/
float randomCirclePoint(float radius) {
	float x, y;
	float xDiff, yDiff;
	do {
		x = randomFloat(radius * 2);
		y = randomFloat(radius * 2);
		
		xDiff = x - radius;
		yDiff = y - radius;
	
	//Try until we get the point inside the circle.
	//This loop will usually run 1-2 times,
	//not a big performance loss but an easy way to generate uniformly random points in a circle
	} while (xDiff * xDiff + yDiff * yDiff > radius * radius);
	
	return pack_float(x, y);
}

//Simply adds the two floats, used in MPI_Fold_Func to count the successful experiments.
float add(float input1, float input2) {
	return input1 + input2;
}

/*
Checks if the given point is in the rectangle.
A single float is taken as input and unpacked into two floats as x and y coordinates.
Used in the MPI_Filter_Func to filter the hits to the rectangle.
*/
bool isInRectangle(float input) {
	float x, y;
	unpack_float(input, &x, &y);
	bool correctY = ((y >= 1) && (y <= 2)) ? true : false;
	bool correctX = ((x - 1.50) * (x - 1.50) <= 2) ? true : false;
	
	return (correctY == true && correctX == true) ? true : false;
}


int main(int argc, char **argv) 
{ 
	MPI_Status s;
	double start, end;
	int numprocs, myid;
	time_t seed = time(NULL); 
	
    	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	start = MPI_Wtime();
	
	if (argc != 2) {
		if (myid == 0)
			printf("Enter the number of experiments as an argument.\n\tCorrect usage: ./main-parallel 1000\n");
		MPI_Finalize(); 
		return 0;
	}
	
	srand(seed + myid); //This gives a different seed to every MPI process so that the same random points are not genereated 
	int numExperiments = atoi(argv[1]);
	float *randomPointRadius, *randomPointPacked, *pointsInRectangle;
	
	if (myid == 0) {
		randomPointRadius = malloc(numExperiments * sizeof(float));
		
		for (int i = 0; i < numExperiments; i++) {
			randomPointRadius[i] = RADIUS;
		}
	}
	
	randomPointPacked = MPI_Map_Func(randomPointRadius, numExperiments, &randomCirclePoint);
	pointsInRectangle = MPI_Filter_Func(randomPointPacked, numExperiments, &isInRectangle);
	float numHits = MPI_Fold_Func(pointsInRectangle, numExperiments, 0.0, &add);
	
	float estimation = (numHits / numExperiments) * PI * 9 / 8;
	
	if (myid == 0) {
		end = MPI_Wtime();
		printf("Estimation of sqrt 2 is: %f\n", estimation);
		printf("Elapsed time: %.10f ms\n", (end - start) * 1000);
	}
	
	
	MPI_Finalize(); 
	return 0;
}

