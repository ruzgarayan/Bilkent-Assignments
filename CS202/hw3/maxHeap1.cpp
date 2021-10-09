#include "maxHeap1.h"

/**
* Author : Rüzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 3
*/

//Mostly taken from the course slides.

// Default constructor 
maxHeap::maxHeap() : size(0) {
}

maxHeap::~maxHeap() {
}

bool maxHeap::heapIsEmpty() const
{
	return (size == 0);
}

bool maxHeap::heapInsert(const HeapPatient& newItem) {
	if (size >= MAX_HEAP)
		return false;

	// Place the new item at the end of the heap
	items[size] = newItem;

	// Trickle new item up to its proper position
	int place = size;
	int parent = (place - 1) / 2;

	//Here, the condition of the while is changed so that
	//the items are first compared with key1 and then key2 if key1's are equal.
	while ((place > 0) && ((items[place].key1 > items[parent].key1) || ((items[place].key1 == items[parent].key1) && (items[place].key2 < items[parent].key2)))) {
		HeapPatient temp = items[parent];
		items[parent] = items[place];
		items[place] = temp;

		place = parent;
		parent = (place - 1) / 2;
	}
	++size;
	return true;
}

bool maxHeap::heapDelete(HeapPatient& rootItem) {
	if (heapIsEmpty())
		return false;
	else {
		rootItem = items[0];
		items[0] = items[--size];
		heapRebuild(0);
	}
	return true;
}

void maxHeap::heapRebuild(int root) {
	int child = 2 * root + 1; 	// index of root's left child, if any
	if (child < size) {
		// root is not a leaf so that it has a left child
		int rightChild = child + 1; 	// index of a right child, if any
		// If root has right child, find larger child

		//Here, the condition of the while is changed so that
		//the items are first compared with key1 and then key2 if key1's are equal.
		if ((rightChild < size) &&
			((items[rightChild].key1 > items[child].key1) || ((items[rightChild].key1 == items[child].key1) && (items[rightChild].key2 < items[child].key2))))
			child = rightChild; 	// index of larger child

		// If root’s item is smaller than larger child, swap values

		//Here, the condition of the while is changed so that
		//the items are first compared with key1 and then key2 if key1's are equal.
		if ((items[root].key1 < items[child].key1) || ((items[root].key1 == items[child].key1) && (items[root].key2 > items[child].key2))) {
			HeapPatient temp = items[root];
			items[root] = items[child];
			items[child] = temp;

			// transform the new subtree into a heap
			heapRebuild(child);
		}
	}
}