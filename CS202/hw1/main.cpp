#include <iostream>
#include "auxArrayFunctions.h"
#include "sorting.h"
#include <chrono>
#include <iomanip> 

using namespace std;

/*
* Title: Algorithm Efficiency and Sorting
* Author: Rüzgar Ayan
* ID: 21801984
* Section: 2
* Assignment: 1
* Description: Contains the main method and
* several other methods to analyse the performance of sorting algorithms.
*/

/*
		A helper method that returns the elapsed time since the last call to the function.
*/
int timer()
{
	static std::chrono::time_point<std::chrono::system_clock > startTime = std::chrono::system_clock::now();

	int elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count();
	startTime = std::chrono::system_clock::now();
	return elapsedTime;
}

/*
		Takes an array "tests" as parameter which contains the sizes of the arrays that will be used as test cases.
		Takes a function pointer "createArray" which creates three identical arrays with the specified size.
*/
void performanceAnalysis(int tests[], int testsLength, void (*createArray)(int*&, int*&, int*&, int))
{
	//Create several arrays to store the obtained data from the tests so that they can be displayed later.
	int* elapsedTimesInsertion = new int[testsLength];
	int* moveCountsInsertion = new int[testsLength];
	int* compCountsInsertion = new int[testsLength];
	int* elapsedTimesMerge = new int[testsLength];
	int* moveCountsMerge = new int[testsLength];
	int* compCountsMerge = new int[testsLength];
	int* elapsedTimesQuick = new int[testsLength];
	int* moveCountsQuick = new int[testsLength];
	int* compCountsQuick = new int[testsLength];

	int* arr1, * arr2, * arr3;
	int moveCount, compCount;
	for (int i = 0; i < testsLength; i++)
	{
		//Create identical arrays.
		createArray(arr1, arr2, arr3, tests[i]);

		//Do tests for insertion sort and store the results in the arrays.
		timer();
		insertionSort(arr1, tests[i], compCount, moveCount);
		elapsedTimesInsertion[i] = timer();
		moveCountsInsertion[i] = moveCount;
		compCountsInsertion[i] = compCount;

		//Do tests for merge sort and store the results in the arrays.
		timer();
		mergeSort(arr2, tests[i], compCount, moveCount);
		elapsedTimesMerge[i] = timer();
		moveCountsMerge[i] = moveCount;
		compCountsMerge[i] = compCount;

		//Do tests for quicksort and store the results in the arrays.
		timer();
		quickSort(arr3, tests[i], compCount, moveCount);
		elapsedTimesQuick[i] = timer();
		moveCountsQuick[i] = moveCount;
		compCountsQuick[i] = compCount;

		//Delete the dynamically created arrays before recreating them in the next pass of the loop.
		delete[] arr1;
		delete[] arr2;
		delete[] arr3;
	}

	//Display the information of Insertion Sort
	cout << "-----------------------------------------------------" << endl;
	cout << "Part c - Time analysis of Insertion Sort" << endl;
	cout << left << setw(20) << "Array Size" 
		<< setw(20) << "Time Elapsed" 
		<< setw(20) << "compCount" 
		<< setw(20) << "moveCount"
		<< endl;

	for (int i = 0; i < testsLength; i++)
	{
		cout << left << setw(20) << tests[i]
			<< setw(20) << elapsedTimesInsertion[i]
			<< setw(20) << compCountsInsertion[i]
			<< setw(20) << moveCountsInsertion[i]
			<< endl;
	}

	//Display the information of Merge Sort
	cout << "-----------------------------------------------------" << endl;
	cout << "Part c - Time analysis of Merge Sort" << endl;
	cout << left << setw(20) << "Array Size"
		<< setw(20) << "Time Elapsed"
		<< setw(20) << "compCount"
		<< setw(20) << "moveCount"
		<< endl;

	for (int i = 0; i < testsLength; i++)
	{
		cout << left << setw(20) << tests[i]
			<< setw(20) << elapsedTimesMerge[i]
			<< setw(20) << compCountsMerge[i]
			<< setw(20) << moveCountsMerge[i]
			<< endl;
	}

	//Display the information of Quicksort
	cout << "-----------------------------------------------------" << endl;
	cout << "Part c - Time analysis of Quick Sort" << endl;
	cout << left << setw(20) << "Array Size"
		<< setw(20) << "Time Elapsed"
		<< setw(20) << "compCount"
		<< setw(20) << "moveCount"
		<< endl;

	for (int i = 0; i < testsLength; i++)
	{
		cout << left << setw(20) << tests[i]
			<< setw(20) << elapsedTimesQuick[i]
			<< setw(20) << compCountsQuick[i]
			<< setw(20) << moveCountsQuick[i]
			<< endl;
	}

	//Delete all the arrays that are used for storing the test data.
	delete[] elapsedTimesInsertion;
	delete[] moveCountsInsertion;
	delete[] compCountsInsertion;
	delete[] elapsedTimesMerge;
	delete[] moveCountsMerge;
	delete[] compCountsMerge;
	delete[] elapsedTimesQuick;
	delete[] moveCountsQuick;
	delete[] compCountsQuick;
} 


/*
		Takes an array "nValues" which contains the sizes of the arrays that will be used as test cases.
		Takes an array "kValuesPercentage" which contains the ratios K/N that will be used as test cases.
*/
void testForQuestion3(int nValues[], int nValuesLength, double kValuesPercentage[], int kValuesPercentageLength)
{
	//Create several arrays to store the obtained data from the tests so that they can be displayed later.
	int* elapsedTimesInsertion = new int[kValuesPercentageLength];
	int* moveCountsInsertion = new int[kValuesPercentageLength];
	int* compCountsInsertion = new int[kValuesPercentageLength];
	int* elapsedTimesMerge = new int[kValuesPercentageLength];
	int* moveCountsMerge = new int[kValuesPercentageLength];
	int* compCountsMerge = new int[kValuesPercentageLength];
	int* elapsedTimesQuick = new int[kValuesPercentageLength];
	int* moveCountsQuick = new int[kValuesPercentageLength];
	int* compCountsQuick = new int[kValuesPercentageLength];

	int* arr1, * arr2, * arr3;
	int compCount, moveCount;
	//First for loop to go over all array sizes.
	for (int n = 0; n < nValuesLength; n++)
	{
		cout << "#######################################" << endl << "Tests for array size "
			 << nValues[n] << endl << "#######################################" << endl;

		//Second for loop to go over all K/N ratios.
		for (int k = 0; k < kValuesPercentageLength; k++)
		{
			//Calculate the K value with array size and K/N ratio.
			int K = nValues[n] * kValuesPercentage[k];
			//Create 3 identical arrays with the calculated K value.
			createNearlySortedArrays(arr1, arr2, arr3, nValues[n], K);

			//Do tests for insertion sort and store the results in the arrays.
			timer();
			insertionSort(arr1, nValues[n], compCount, moveCount);
			elapsedTimesInsertion[k] = timer();
			moveCountsInsertion[k] = moveCount;
			compCountsInsertion[k] = compCount;

			//Do tests for merge sort and store the results in the arrays.
			timer();
			mergeSort(arr2, nValues[n], compCount, moveCount);
			elapsedTimesMerge[k] = timer();
			moveCountsMerge[k] = moveCount;
			compCountsMerge[k] = compCount;

			//Do tests for quicksort and store the results in the arrays.
			timer();
			quickSort(arr3, nValues[n], compCount, moveCount);
			elapsedTimesQuick[k] = timer();
			moveCountsQuick[k] = moveCount;
			compCountsQuick[k] = compCount;

			//Delete the dynamically created arrays before recreating them in the next pass of the loop.
			delete[] arr1;
			delete[] arr2;
			delete[] arr3;
		}	

		//Display the information of Insertion Sort
		cout << "-----------------------------------------------------" << endl;
		cout << "Question 3 - Nearly Sorted Array with Insertion Sort" << endl;
		cout << left << setw(20) << "(K/N)"
			<< setw(20) << "Time Elapsed"
			<< setw(20) << "compCount"
			<< setw(20) << "moveCount"
			<< endl;

		for (int i = 0; i < kValuesPercentageLength; i++)
		{
			cout << left << setw(20) << kValuesPercentage[i]
				<< setw(20) << elapsedTimesInsertion[i]
				<< setw(20) << compCountsInsertion[i]
				<< setw(20) << moveCountsInsertion[i]
				<< endl;
		}

		//Display the information of Merge Sort
		cout << "-----------------------------------------------------" << endl;
		cout << "Question 3 - Nearly Sorted Array with Merge Sort" << endl;
		cout << left << setw(20) << "(K/N)"
			<< setw(20) << "Time Elapsed"
			<< setw(20) << "compCount"
			<< setw(20) << "moveCount"
			<< endl;

		for (int i = 0; i < kValuesPercentageLength; i++)
		{
			cout << left << setw(20) << kValuesPercentage[i]
				<< setw(20) << elapsedTimesMerge[i]
				<< setw(20) << compCountsMerge[i]
				<< setw(20) << moveCountsMerge[i]
				<< endl;
		}

		//Display the information of Quicksort
		cout << "Question 3 - Nearly Sorted Array with Quick Sort" << endl;
		cout << left << setw(20) << "(K/N)"
			<< setw(20) << "Time Elapsed"
			<< setw(20) << "compCount"
			<< setw(20) << "moveCount"
			<< endl;

		for (int i = 0; i < kValuesPercentageLength; i++)
		{
			cout << left << setw(20) << kValuesPercentage[i]
				<< setw(20) << elapsedTimesQuick[i]
				<< setw(20) << compCountsQuick[i]
				<< setw(20) << moveCountsQuick[i]
				<< endl;
		}
	}

	//Delete all the arrays that are used for storing the test data.
	delete[] elapsedTimesInsertion;
	delete[] moveCountsInsertion;
	delete[] compCountsInsertion;
	delete[] elapsedTimesMerge;
	delete[] moveCountsMerge;
	delete[] compCountsMerge;
	delete[] elapsedTimesQuick;
	delete[] moveCountsQuick;
	delete[] compCountsQuick;
}

/*
	Simply sorts the given small array and displays the statistics.
*/
void testForPartB()
{
	int arr1[] = {9, 5, 8, 15, 16, 6, 3, 11, 18, 0, 14, 17, 2, 9, 11, 7};
	int arr2[] = {9, 5, 8, 15, 16, 6, 3, 11, 18, 0, 14, 17, 2, 9, 11, 7};
	int arr3[] = {9, 5, 8, 15, 16, 6, 3, 11, 18, 0, 14, 17, 2, 9, 11, 7};
	int compCount = 0, moveCount = 0;
	int size = 16;

	insertionSort(arr1, size, compCount, moveCount);
	cout << "For Insertion Sort, compCount = " << compCount << ", moveCount = " << moveCount << endl;
	displayArray(arr1, size);

	compCount = 0; moveCount = 0;
	mergeSort(arr2, size, compCount, moveCount);
	cout << "\nFor Merge Sort, compCount = " << compCount << ", moveCount = " << moveCount << endl;
	displayArray(arr2, size);

	compCount = 0; moveCount = 0;
	quickSort(arr3, size, compCount, moveCount);
	cout << "\nFor Quicksort, compCount = " << compCount << ", moveCount = " << moveCount << endl;
	displayArray(arr3, size);
}

/*
	Use the other methods and display the results of tests.
*/
int main()
{
	cout << "Question 2, Part(b)" << endl << endl;
	testForPartB();

	cout << endl << "Question 2, Part(c)" << endl << endl;
	int testsForQuestion2[] = {5000, 10000, 15000, 20000, 25000};
	int testsForQuestion2Length = 5;

	cout		<< "#######################################" << endl << "Test with Random Arrays" 
		<< endl << "#######################################" << endl;
	performanceAnalysis(testsForQuestion2, testsForQuestion2Length, createRandomArrays);

	cout		<< "#######################################" << endl << "Test with Already Sorted Arrays"
		<< endl << "#######################################" << endl;
	performanceAnalysis(testsForQuestion2, testsForQuestion2Length, createAlreadySortedArrays);

	cout << endl << "Question 3" << endl << endl;

	int nValues[] = {10000, 40000, 70000};
	int nValuesLength = 3;
	double kValuesPercentage[] = {0.0001, 0.001, 0.01, 0.02, 0.03, 0.04, 0.05, 0.10, 0.15, 0.20, 0.25};
	int kValuesPercentageLength = 11;

	testForQuestion3(nValues, nValuesLength, kValuesPercentage, kValuesPercentageLength);

	return 0;
}
