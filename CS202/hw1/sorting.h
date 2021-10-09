/*
* Title: Algorithm Efficiency and Sorting
* Author: Rüzgar Ayan
* ID: 21801984
* Section: 2
* Assignment: 1
* Description: The top three methods are the required ones,
* others are helper methods and methods that will be used in recursive calls.
*/

void insertionSort(int* arr, int size, int& compCount, int& moveCount);
void quickSort(int* arr, int size, int& compCount, int& moveCount);
void mergeSort(int* arr, int size, int& compCount, int& moveCount);

void swap(int& x, int& y);
void quickSort(int theArray[], int first, int last, int& compCount, int& moveCount);
void partition(int theArray[], int first, int last, int& pivotIndex, int& compCount, int& moveCount);
void mergeSort(int theArray[], int size, int first, int last, int& compCount, int& moveCount);
void merge(int theArray[], int size, int first, int mid, int last, int& compCount, int& moveCount);
void choosePivot(int theArray[], int first, int last, int& compCount, int& moveCount);

