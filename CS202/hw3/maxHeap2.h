#pragma once

/**
* Author : Rüzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 3
*/

#include <string>
using namespace std;

//defined a struct to keep 2 keys
struct KeyedItem
{
	int key1; //will be the priority of patient
	int key2; //will be the id of patient since id is also used for comparing when the priorities are the same.
	int item;
};
typedef KeyedItem HeapPatient;

//Taken from the course slides.
class maxHeap
{
public:
	maxHeap();
	~maxHeap(); 
	bool heapIsEmpty() const;
	bool heapInsert(const HeapPatient& newItem);
	bool heapDelete(HeapPatient& rootItem);
protected:
	void heapRebuild(int root);

private:
	HeapPatient* items;
	int          size;
	int			 maxSize;

};

