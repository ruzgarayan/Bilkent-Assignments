#include "sorting.h"

/*
* Title: Algorithm Efficiency and Sorting
* Author: Rüzgar Ayan
* ID: 21801984
* Section: 2
* Assignment: 1
* Description: A collection of different sorting algorithms 
* that counts the number of comparisons and number of moves.
*/

void insertionSort(int* arr, int size, int& compCount, int& moveCount) {
	//reset the values to 0 in case that they are not given as 0.
	compCount = 0;
	moveCount = 0;

	for (int unsorted = 1; unsorted < size; ++unsorted) {

		int nextItem = arr[unsorted];
		moveCount++;

		int loc = unsorted;

		for (; (loc > 0) && (arr[loc - 1] > nextItem); --loc)
		{
			compCount++;
			arr[loc] = arr[loc - 1];
			moveCount++;
		}
		if (loc > 0) //If loc > 0, a comparison is made just before exiting the loop.
			compCount++;	

		arr[loc] = nextItem;
		moveCount++;
	}
}

void quickSort(int* arr, int size, int& compCount, int& moveCount)
{
	//reset the values to 0 before sending them to the actual method.
	compCount = 0;
	moveCount = 0;
	quickSort(arr, 0, size - 1, compCount, moveCount);
}


void mergeSort(int* arr, int size, int& compCount, int& moveCount)
{
	//reset the values to 0 before sending them to the actual method.
	compCount = 0;
	moveCount = 0;
	mergeSort(arr, size, 0, size - 1, compCount, moveCount);
}

//Swapping method that uses 3 moves.
void swap(int& x, int& y) {
	int temp = x;
	x = y;
	y = temp;
}

void quickSort(int theArray[], int first, int last, int& compCount, int& moveCount) {

	int pivotIndex;

	if (first < last) {
		partition(theArray, first, last, pivotIndex, compCount, moveCount);

		quickSort(theArray, first, pivotIndex - 1, compCount, moveCount);
		quickSort(theArray, pivotIndex + 1, last, compCount, moveCount); 
	}
}

// place pivot in theArray[first], no comparison or data move
void choosePivot(int theArray[], int first, int last, int& compCount, int& moveCount) {
	//To choose the first element as pivot, we don't need to do anyting as it is already at where it should be.
}

void partition(int theArray[], int first, int last,
	int& pivotIndex, int& compCount, int& moveCount) {
	choosePivot(theArray, first, last, compCount, moveCount);

	int pivot = theArray[first];
	moveCount++;
	int lastS1 = first;          
	int firstUnknown = first + 1; 

	for (; firstUnknown <= last; ++firstUnknown) {

		compCount++;
		if (theArray[firstUnknown] < pivot) {
			++lastS1;
			swap(theArray[firstUnknown], theArray[lastS1]);
			moveCount += 3;	//from swapping
		}
	}
	swap(theArray[first], theArray[lastS1]);
	moveCount += 3;	//from swapping

	pivotIndex = lastS1;
}

void mergeSort(int theArray[], int size, int first, int last, int& compCount, int& moveCount) {

	if (first < last) {

		int mid = (first + last) / 2; 

		mergeSort(theArray, size, first, mid, compCount, moveCount);

		mergeSort(theArray, size, mid + 1, last, compCount, moveCount);

		merge(theArray, size, first, mid, last, compCount, moveCount);
	}
}

void merge(int theArray[], int size, int first, int mid, int last, int& compCount, int& moveCount) {

	int* tempArray = new int[size]; 	

	int first1 = first; 	
	int last1 = mid; 		
	int first2 = mid + 1;	
	int last2 = last;		
	int index = first1; 

	for (; (first1 <= last1) && (first2 <= last2); ++index) {
		compCount++;
		if (theArray[first1] < theArray[first2]) {
			tempArray[index] = theArray[first1];
			moveCount++;
			++first1;
		}
		else {
			moveCount++;
			tempArray[index] = theArray[first2];
			++first2;
		}
	}
	for (; first1 <= last1; ++first1, ++index)
	{
		tempArray[index] = theArray[first1];
		moveCount++;
	}

	for (; first2 <= last2; ++first2, ++index)
	{
		tempArray[index] = theArray[first2];
		moveCount++;
	}

	for (index = first; index <= last; ++index)
	{
		theArray[index] = tempArray[index];
		moveCount++;
	}

	delete[] tempArray;
}