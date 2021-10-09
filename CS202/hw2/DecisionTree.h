#pragma once

#include "DecisionTreeNode.h"
#include <string>

using namespace std;

/**
* Author : RüzgarAyan
* ID: 21801984
* Section : 2
* Assignment : 2
*/

class DecisionTree
{
public:
	DecisionTree();
	~DecisionTree();

	void train(const bool** data, const int* labels, const
		int numSamples, const int numFeatures);
	void train(const string fileName, const int numSamples,
		const int numFeatures);
	int predict(const bool* data);
	double test(const bool** data, const int* labels, const
		int numSamples);
	double test(const string fileName, const int numSamples);
	void print();
private:
	DecisionTreeNode* root;
	int numFeatures;
	void train(const bool** data, const int* labels, const
		int numSamples, const int numFeatures, DecisionTreeNode* currentNode,
		const bool* usedSamples, const bool* usedFeatures);
	void makePureLeaf(DecisionTreeNode* node, const int classLabel);
	void makeNonPureLeaf(DecisionTreeNode* node, const int* labels, const int numSamples, const int numFeatures, const bool* usedSamples);
	void print(const DecisionTreeNode* currentNode, const int currentLevel);
};

