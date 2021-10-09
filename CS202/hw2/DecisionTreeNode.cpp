#include "DecisionTreeNode.h"

/**
* Author : R�zgarAyan
* ID: 21801984
* Section : 2
* Assignment : 2
*/

DecisionTreeNode::DecisionTreeNode()
{
	classDecision = 1;
	featureIndex = -1;
	isLeaf = true;
	leftChild = nullptr;
	rightChild = nullptr;
}

DecisionTreeNode::~DecisionTreeNode()
{
	delete leftChild;
	delete rightChild;
}