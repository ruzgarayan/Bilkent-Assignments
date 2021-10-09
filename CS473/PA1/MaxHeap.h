#pragma once

struct Node {
	int key;
	int data;
	int hIndex;
};

typedef struct Node* Node;

struct MaxHeap {
	Node *elements;
	int maxSize;
	int size;
};

typedef struct MaxHeap *MaxHeap;

MaxHeap initHeap(int maxSize);
MaxHeap initHeapFromArray(Node* elements, int size);
int extractMax(MaxHeap h);
void deleteHeapFromArray(MaxHeap h);
void insert(MaxHeap h, int key);
void increaseKey(MaxHeap h, int index, int key);
void decreaseKey(MaxHeap h, int index, int key);
void printHeap(MaxHeap h);
void heapify(MaxHeap h, int index);