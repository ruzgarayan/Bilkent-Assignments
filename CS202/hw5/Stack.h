#pragma once
#include "SimpleLinkedList.h"

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 5
*/

class Stack
{
public:
	Stack();
	int size() const;
	bool isEmpty();
	int pop();
	void push(const int item);
private:
	SimpleLinkedList list;
};

