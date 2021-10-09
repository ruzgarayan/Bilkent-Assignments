#pragma once

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 5
*/

struct Node
{
	int item;
	Node* next;
};

class SimpleLinkedList
{
public:
	SimpleLinkedList();
	~SimpleLinkedList();
	void operator=(const SimpleLinkedList& toCopy);
	bool add(const int item);
	bool deleteHead(int& item);
	bool deleteLast(int& item);
	bool isPresent(const int item) const;
	int size() const;
	int* getListAsArray(int& length) const;
private:
	Node* head;
	int length;
};

