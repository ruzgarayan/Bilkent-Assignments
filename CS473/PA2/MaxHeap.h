#pragma once

struct Node {
	double key;
	int data;
	int hIndex;
};

typedef struct Node* Node;

struct MaxHeap {
	Node* elements;
	int maxSize;
	int size;
};

typedef struct MaxHeap* MaxHeap;

MaxHeap initMaxHeap(int maxSize);
MaxHeap initMaxHeapFromArray(Node* elements, int size);
Node extractMax(MaxHeap h);
void deleteMaxHeapFromArray(MaxHeap h);
void insertMaxHeap(MaxHeap h, double key, int data);
void increaseKeyMaxHeap(MaxHeap h, int index, double key);
void printMaxHeap(MaxHeap h);
void heapifyMaxHeap(MaxHeap h, int index);
int sizeMaxHeap(MaxHeap h);