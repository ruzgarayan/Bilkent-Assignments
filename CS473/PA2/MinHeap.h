#pragma once

#include "MaxHeap.h"

struct MinHeap {
	Node* elements;
	int maxSize;
	int size;
};

typedef struct MinHeap* MinHeap;

MinHeap initMinHeap(int maxSize);
MinHeap initMinHeapFromArray(Node* elements, int size);
Node extractMin(MinHeap h);
void deleteMinHeapFromArray(MinHeap h);
void insertMinHeap(MinHeap h, double key, int data);
void decreaseKeyMinHeap(MinHeap h, int index, double key);
void printMinHeap(MinHeap h);
void heapifyMinHeap(MinHeap h, int index);
int sizeMinHeap(MinHeap h);