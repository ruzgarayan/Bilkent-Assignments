#pragma once
#include "SimpleLinkedList.h"

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 5
*/

class Queue
{
public:
	Queue();
	int size() const;
	bool isEmpty();
	int dequeue();
	void enqueue(const int item);
private:
	SimpleLinkedList list;
};

