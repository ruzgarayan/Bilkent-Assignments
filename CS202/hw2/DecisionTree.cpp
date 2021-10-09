#include "DecisionTree.h"
#include "DecisionTreeNode.h" 
#include "EntropyFunctions.h"
#include <iostream>
#include <fstream>

using namespace std;

/**
* Author : RüzgarAyan
* ID: 21801984
* Section : 2
* Assignment : 2
*/

DecisionTree::DecisionTree()
{
	root = NULL;
	numFeatures = 0; //Will be set to its value when trained.
}

DecisionTree::~DecisionTree()
{
	delete root; //All the nodes will be deleted by the help of the destructor of DecisionTreeNode.
}

void DecisionTree::train(const bool** data, const int* labels, const
	int numSamples, const int numFeatures)
{
	this->numFeatures = numFeatures;

	//The features already used up until that node will be set to true.
	//All are false at the beginning.
	bool* usedFeatures = new bool[numFeatures];
	for (int i = 0; i < numFeatures; i++)
		usedFeatures[i] = false;

	//The samples that will be used in that node will be set to true,
	//as same as the one in calculateInformationGain function.
	//All are true at the beginning
	bool* usedSamples = new bool[numSamples];
	for (int i = 0; i < numSamples; i++)
		usedSamples[i] = true;

	root = new DecisionTreeNode();
	train(data, labels, numSamples, numFeatures, root, usedSamples, usedFeatures);

	delete[] usedFeatures;
	delete[] usedSamples;
}

void DecisionTree::train(const bool** data, const int* labels, const
	int numSamples, const int numFeatures, DecisionTreeNode* currentNode,
	const bool* usedSamples, const bool* usedFeatures)
{
	/*
	cout << "Train beginning" << endl;
	for (int i = 0; i < numSamples; i++)
	{
		if (usedSamples[i])
			cout << i << endl;
	}
	cout << endl;
	*/


	//Check if the samples in the node is pure
	bool isPure = true;
	int classOfPure = 0;
	for (int i = 0; i < numSamples; i++)
	{
		if (usedSamples[i] && classOfPure == 0)
			classOfPure = labels[i];
		else if (usedSamples[i] && labels[i] != classOfPure)
		{
			isPure = false;
			break;
		}
	}
	if (isPure) //This is a leaf node and we can stop the recursion.
	{
		makePureLeaf(currentNode, classOfPure);
		return; //stop recursion for this path.
	}

	//Check if there are features left to split
	bool featuresLeft = false;
	for (int i = 0; i < numFeatures; i++)
	{
		if (!usedFeatures[i])
		{
			featuresLeft = true;
			break; 
		}
	}
	//This is a leaf node and we can stop the recursion.
	if (!featuresLeft)
	{
		makeNonPureLeaf(currentNode, labels, numSamples, numFeatures, usedSamples);
		return; //stop recursion for this path.
	}

	//After that point, we know we are going to do a split
	//And we are going to try to find the best split
	int bestInformationGain = -99999999; //std::numeric_limits<int>::min() didn't work on dijkstra 
	int bestSplitIndex = 0;
	double informationGain;
	//cout << "Information Gains:" << endl;
	for (int i = 0; i < numFeatures; i++)
	{
		if (!usedFeatures[i])
		{
			informationGain = calculateInformationGain(data, labels, numSamples, numFeatures, usedSamples, i);
			//cout << informationGain << endl;
			if (informationGain > bestInformationGain)
			{
				bestInformationGain = informationGain;
				bestSplitIndex = i;
			}
		}
	}
	currentNode->isLeaf = false;
	currentNode->featureIndex = bestSplitIndex;
	currentNode->leftChild = new DecisionTreeNode();
	currentNode->rightChild = new DecisionTreeNode();

	bool* newUsedFeatures = new bool[numFeatures];
	for (int i = 0; i < numFeatures; i++)
		newUsedFeatures[i] = usedFeatures[i];
	newUsedFeatures[bestSplitIndex] = true;

	int numLeftSamples = 0;
	int numRightSamples = 0;

	//Put the samples that are currently used in this node
	//with their split feature true into the samples of the left child
	bool* leftUsedSamples = new bool[numSamples];
	for (int i = 0; i < numSamples; i++) {
		if (usedSamples[i] && data[i][bestSplitIndex])
		{
			leftUsedSamples[i] = true;
			numLeftSamples++;
		}
		else
			leftUsedSamples[i] = false;
	}

	//Put the samples that are currently used in this node
	//with their split feature false into the samples of the right child
	bool* rightUsedSamples = new bool[numSamples];
	for (int i = 0; i < numSamples; i++) {
		if (usedSamples[i] && !data[i][bestSplitIndex])
		{
			rightUsedSamples[i] = true;
			numRightSamples++;
		}
		else
			rightUsedSamples[i] = false;
	}

	if (numLeftSamples == 0 || numRightSamples == 0)
	{
		makeNonPureLeaf(currentNode, labels, numSamples, numFeatures, usedSamples);
		return; //stop recursion for this path.
	}

	train(data, labels, numSamples, numFeatures, currentNode->leftChild,
		leftUsedSamples, newUsedFeatures);
	delete[] leftUsedSamples;
	train(data, labels, numSamples, numFeatures, currentNode->rightChild,
		rightUsedSamples, newUsedFeatures);
	delete[] rightUsedSamples;

	delete[] newUsedFeatures;
}

void DecisionTree::makePureLeaf(DecisionTreeNode* node, const int classLabel)
{
	//cout << "make pure leaf: " << classLabel << endl;
	node->isLeaf = true;
	node->classDecision = classLabel;
}
void DecisionTree::makeNonPureLeaf(DecisionTreeNode* node, const int* labels, const int numSamples, const int numFeatures, const bool* usedSamples)
{
	node->isLeaf = true;

	//Find the number of samples from each classes.
	int* classCounts = new int[numSamples];
	for (int i = 0; i < numSamples; i++)
		classCounts[i] = 0;
	for (int i = 0; i < numSamples; i++) {
		if (usedSamples[i] == 1)
			classCounts[labels[i] - 1]++;
	}

	//Find the dominant class.
	int biggestCount = 0;
	int chosenIndex = 0;
	for (int i = 0; i < numSamples; i++) {
		if (classCounts[i] > biggestCount) {
			biggestCount = classCounts[i];
			chosenIndex = i + 1; //To convert from 0-indexed to 1-indexed.
		}
	}
	node->classDecision = chosenIndex;
	delete[] classCounts;
}

void DecisionTree::train(const string fileName, const int numSamples,
	const int numFeatures)
{
	ifstream in(fileName);
	
	//Create the appropriate dynamic arrays.
	bool** data = new bool*[numSamples];
	for (int i = 0; i < numSamples; i++)
		data[i] = new bool[numFeatures];
	int* labels = new int[numSamples];

	//Parse from the file.
	for (int i = 0; i < numSamples; i++)
	{
		for (int j = 0; j < numFeatures; j++)
			in >> data[i][j];
		in >> labels[i];
	}
	in.close();  
	//Call the original train function with the parsed data
	train(const_cast<const bool**>(data), labels, numSamples, numFeatures);

	//Delete dynamic arrays.
	for (int i = 0; i < numSamples; i++)
		delete[] data[i];
	delete[] data;
	delete[] labels;
}
int DecisionTree::predict(const bool* data)
{
	if (root == NULL)
		return -1;

	//Start from the root.
	DecisionTreeNode* currentNode = root;
	//Go to the correct child until a leaf node is reached. 
	while (!currentNode->isLeaf)
	{
		if (data[currentNode->featureIndex])
			currentNode = currentNode->leftChild;
		else
			currentNode = currentNode->rightChild;
	}
	return currentNode->classDecision;
}
double DecisionTree::test(const bool** data, const int* labels, const
	int numSamples)
{
	if (root == NULL)
		return -1;

	int correctPredictions = 0;

	//count the correct predictions
	for (int i = 0; i < numSamples; i++)
	{
		if (predict(data[i]) == labels[i])
		{
			//cout << i << endl;
			correctPredictions++;
		}
	}
	return (correctPredictions + 0.0) / numSamples;
}
double DecisionTree::test(const string fileName, const int numSamples)
{
	if (root == NULL)
		return -1;

	ifstream in(fileName);

	//create dynamic arrays to parse the data in
	bool** data = new bool* [numSamples];
	for (int i = 0; i < numSamples; i++)
		data[i] = new bool[numFeatures];
	int* labels = new int[numSamples];

	//parse the data from the file
	for (int i = 0; i < numSamples; i++)
	{
		for (int j = 0; j < numFeatures; j++)
			in >> data[i][j];
		in >> labels[i];
	}
	in.close();

	//call the original test function.
	double result = test(const_cast<const bool**>(data), labels, numSamples);

	//delete the dynamic arrays and return the resuslt.
	for (int i = 0; i < numSamples; i++)
		delete[] data[i];
	delete[] data;
	delete[] labels;
	return result;
}
void DecisionTree::print()
{
	//start printing from the root and with no tabs.
	print(root, 0);
}
void DecisionTree::print(const DecisionTreeNode* currentNode, const int currentLevel)
{
	if (currentNode->isLeaf)
	{
		//print the data for the current node.
		for (int i = 0; i < currentLevel; i++)
			cout << "\t";
		cout << "class=" << currentNode->classDecision << endl;
	}
	else
	{
		//print the data for the current node.
		for (int i = 0; i < currentLevel; i++)
			cout << "\t";
		cout << "feature=" << currentNode->featureIndex << endl;

		//call print with left child and incremented tab level.
		print(currentNode->leftChild, currentLevel + 1);
		
		//call print with right child and incremented tab level.
		print(currentNode->rightChild, currentLevel + 1);
	}
}