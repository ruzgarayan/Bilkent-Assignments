#include "SimpleLinkedList.h"

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 5
*/

SimpleLinkedList::SimpleLinkedList()
{
	head = nullptr;
	length = 0;
}

SimpleLinkedList::~SimpleLinkedList()
{
	//Delete all nodes
	while (head != nullptr)
	{
		Node* temp = head->next;
		delete head;
		head = temp;
	}
}
void SimpleLinkedList::operator=(const SimpleLinkedList& toCopy)
{
	//First delete all the nodes.
	while (head != nullptr)
	{
		Node* temp = head->next;
		delete head;
		head = temp;
	}
	head = nullptr;
	length = 0;

	//Then, add every item to the list.
	Node* ptr = toCopy.head;
	for (int i = 0; i < toCopy.length; i++)
	{
		add(ptr->item);
		ptr = ptr->next;
	}
}

bool SimpleLinkedList::add(const int item)
{
	//If list is empty, initialize head as a new node.
	if (head == nullptr)
	{
		head = new Node();
		head->item = item;
		head->next = nullptr;
		length++;
		return true;
	}

	//If list is not empty,
	//Create a new node with the given item
	//and make it the new head.
	Node* newNode = new Node();
	newNode->item = item;
	newNode->next = head;
	head = newNode;
	length++;
	return true;
}

bool SimpleLinkedList::deleteHead(int& item)
{
	if (size() == 0)
		return false;
	else
	{
		//Delete head and set head->next as the new head.
		item = head->item;
		Node* temp = head->next;
		delete head;
		head = temp;
		length--;
	}
}

bool SimpleLinkedList::deleteLast(int& item)
{
	if (size() == 0)
		return false;

	//Special case for size == 1
	if (size() == 1)
	{
		item = head->item;
		delete head;
		head = nullptr;

		length--;
		return true;
	}

	Node* ptr = head;
	//Find the node just before the last one.
	for (int i = 0; i < size() - 2; i++)
	{
		ptr = ptr->next;
	}
	Node* lastNode = ptr->next;
	item = lastNode->item;
	delete lastNode; //Delete the last node.
	ptr->next = nullptr; //Set the next of ptr to null.

	length--;
	return true;
}

//Returns if the given item is present in the list or not.
//(I thought I would use this, but I didn't need it in my implementation.)
bool SimpleLinkedList::isPresent(const int item) const
{
	Node* ptr = head;
	while (ptr != nullptr)
	{
		if (ptr->item == item)
			return true;
		ptr = ptr->next;
	}
	return false;
}

int SimpleLinkedList::size() const
{
	return length;
}

//Returns the integers in the linked list as an dynamic array.
int* SimpleLinkedList::getListAsArray(int& length) const
{
	length = this->length;
	int* arr = new int[length];
	if (length == 0)
		return arr;

	Node* ptr = head;
	for (int i = 0; i < length; i++)
	{
		arr[i] = ptr->item;
		ptr = ptr->next;
	}

	return arr;
}
