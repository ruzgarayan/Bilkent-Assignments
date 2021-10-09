#include "MaxHeap.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

MaxHeap initHeap(int maxSize)
{
	MaxHeap h;
	h = malloc(sizeof(struct MaxHeap));
	h->elements = malloc(sizeof(struct Node) * (maxSize + 1));
	h->maxSize = maxSize;
	h->size = 0;

	return h;
}

MaxHeap initHeapFromArray(Node* elements, int maxSize)
{
	MaxHeap h = initHeap(maxSize + 1);
	h->maxSize = maxSize;
	h->size = maxSize;

	int i;
	for (i = 0; i < maxSize; i++)
	{
		h->elements[i + 1] = elements[i];
		h->elements[i + 1]->hIndex = i + 1;
	}

	for (i = maxSize / 2; i >= 1; i--) heapify(h, i);
	return h;
}

void deleteHeapFromArray(MaxHeap h)
{
	free(h->elements);
	free(h);
}


int extractMax(MaxHeap h)
{
	int max = h->elements[1]->data;
	h->elements[1] = h->elements[h->size];
	h->elements[1]->hIndex = 1;
	h->size = h->size - 1;
	heapify(h, 1);

	return max;
}

void insert(MaxHeap h, int key)
{
	if (h->size >= h->maxSize)
		return;

	h->size = h->size + 1;
	int i = h->size;

	while (i > 1 && h->elements[(int)floor(i / 2.0)]->key < key)
	{
		h->elements[i] = h->elements[(int)floor(i / 2.0)];
		i = (int)floor(i / 2.0);
	}

	Node newElement = malloc(sizeof(Node));
	newElement->key = key;
	h->elements[i] = newElement;
}

void increaseKey(MaxHeap h, int i, int key)
{
	if (key < h->elements[i]->key)
		return;
	
	Node temp = h->elements[i];

	int iOver2 = (int)floor(i / 2.0);
	while (i > 1 && h->elements[iOver2]->key < key)
	{
		h->elements[i] = h->elements[iOver2];
		h->elements[i]->hIndex = i;
		i = iOver2;
		iOver2 = (int)floor(i / 2.0);
	}

	h->elements[i] = temp;
	h->elements[i]->key = key;
	h->elements[i]->hIndex = i;
}

void decreaseKey(MaxHeap h, int i, int key)
{
	if (key > h->elements[i]->key)
		return;

	h->elements[i]->key = key;
	heapify(h, i);
}

void heapify(MaxHeap h, int i)
{
	int largest = i;
	int n = h->size;

	if (2 * i <= n && h->elements[2 * i]->key > h->elements[i]->key)
		largest = 2 * i;
	if (2 * i + 1 <= n && h->elements[2 * i + 1]->key > h->elements[largest]->key)
		largest = 2 * i + 1;

	if (largest != i)
	{
		Node temp = h->elements[i];
		h->elements[i] = h->elements[largest];
		h->elements[i]->hIndex = i;
		h->elements[largest] = temp;
		h->elements[largest]->hIndex = largest;
		heapify(h, largest);
	}
}

void printHeap(MaxHeap h)
{
	int i;
	for (i = 1; i <= h->size; i++)
	{
		printf("At index %d, the node is {key=%d, data=%d, hIndex=%d}\n",i,h->elements[i]->key, h->elements[i]->data, h->elements[i]->hIndex);
	}
}

int tryHeap()
{
	/*
	MaxHeap h = initHeap(10);
	insert(h, 5,5);
	insert(h, 15,15);
	insert(h, 10,10);
	increaseKey(h,3,40);
	int max = extractMax(h);
	printf("%d ", max);
	max = extractMax(h);
	printf("%d ", max);
	insert(h, 15,15);
	max = extractMax(h);
	printf("%d ", max);
	max = extractMax(h);
	printf("%d ", max);
	*/

	Node* nodes = malloc(sizeof(Node) * 5);
	int i;
	for (i = 0; i < 5; i++)
	{
		nodes[i] = malloc(sizeof(struct Node));
		nodes[i]->key = i;
		nodes[i]->key = i + 1;
	}
	MaxHeap h = initHeapFromArray(nodes, 5);

	int max = extractMax(h);
	printf("%d\n", max);
	max = extractMax(h);
	printf("%d\n", max);
	max = extractMax(h);
	printf("%d\n", max);
	max = extractMax(h);
	printf("%d\n", max);
	max = extractMax(h);
	printf("%d\n", max);

	return 0;
}