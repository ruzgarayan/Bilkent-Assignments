#pragma once

/**
* Author : RüzgarAyan
* ID: 21801984
* Section : 2
* Assignment : 2
*/

class DecisionTreeNode
{
public:
	DecisionTreeNode();
	~DecisionTreeNode();

private:
	bool isLeaf;
	int featureIndex;
	int classDecision;

	DecisionTreeNode* leftChild;
	DecisionTreeNode* rightChild;

	friend class DecisionTree;
};

