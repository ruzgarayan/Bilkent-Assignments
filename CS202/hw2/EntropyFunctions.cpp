#include "EntropyFunctions.h"
#include <math.h>
#include <iostream>

/**
* Author : RüzgarAyan
* ID: 21801984
* Section : 2
* Assignment : 2
*/

double calculateEntropy(const int* classCounts, const int numClasses)
{
	double entropy = 0;

	//First compute the total number of samples.
	int numOfSamples = 0;
	for (int i = 0; i < numClasses; i++)
		numOfSamples += classCounts[i];

	//Sum P_i*log(P_i) over all i's
	for (int i = 0; i < numClasses; i++)
	{
		if (classCounts[i] != 0) //Don't consider this case since log(0) is undefined.
			entropy += (static_cast<double>(classCounts[i]) / static_cast<double>(numOfSamples)) *
			log2(static_cast<double>(classCounts[i]) / static_cast<double>(numOfSamples));
		//I have used a lot of casting which was probably not required at all,
		//but I wanted to guarantee that the function works. 
	}
	entropy = -entropy; //From the minus in the definition of entropy in a node.

	return entropy;
}

double calculateInformationGain(const bool** data, const int* labels,
	const int numSamples, const int numFeatures, const bool* usedSamples,
	const int featureId)
{
	double nodeEntropy;
	double leftChildEntropy, rightChildEntropy;
	double splitEntropy;

	//Find how many classes there are in the data set.
	//(Find the biggest element in the labels)
	int numOfClasses = 1;
	for (int i = 0; i < numSamples; i++)
	{
		if (labels[i] > numOfClasses)
			numOfClasses = labels[i];
	}

	//Find the number of samples for each class label
	int* classCounts = new int[numOfClasses];
	for (int i = 0; i < numOfClasses; i++)
		classCounts[i] = 0;
	for (int i = 0; i < numSamples; i++)
	{
		if (usedSamples[i])
			classCounts[labels[i] - 1] += 1;
	}
	nodeEntropy = calculateEntropy(classCounts, numOfClasses);

	//Number of samples in the left and the right splits.
	int leftCount = 0, rightCount = 0;

	//Now compute the entropies of the left and right children.

	//Reset the classCounts array and this time
	//put the samples used only in the left split.
	for (int i = 0; i < numOfClasses; i++)
		classCounts[i] = 0;
	for (int i = 0; i < numSamples; i++)
		if (usedSamples[i] && data[i][featureId])
		{ 
			classCounts[labels[i] - 1]++;
			leftCount++;
		}
	leftChildEntropy = calculateEntropy(classCounts, numOfClasses);

	//Reset the classCounts array and this time
	//put the samples used only in the right split.
	for (int i = 0; i < numOfClasses; i++)
		classCounts[i] = 0;
	for (int i = 0; i < numSamples; i++)
		if (usedSamples[i] && !data[i][featureId])
		{
			classCounts[labels[i] - 1]++;
			rightCount++;
		}
	rightChildEntropy = calculateEntropy(classCounts, numOfClasses);

	delete[] classCounts;

	//Compute the splitEntropy from the definition given in the assignment pdf.
	splitEntropy = ((leftCount + 0.0) / (0.0 + leftCount + rightCount)) * leftChildEntropy +
		((rightCount + 0.0) / (0.0 + leftCount + rightCount)) * rightChildEntropy;
	return nodeEntropy - splitEntropy;
}