#include "MaxHeap.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

MaxHeap initMaxHeap(int maxSize)
{
	MaxHeap h;
	h = malloc(sizeof(struct MaxHeap)); if (h == NULL) return NULL;
	h->elements = malloc(sizeof(struct Node) * (maxSize + 1)); if (h->elements == NULL) return NULL;
	h->maxSize = maxSize;
	h->size = 0;

	return h;
}

MaxHeap initMaxHeapFromArray(Node* elements, int maxSize)
{
	MaxHeap h = initMaxHeap(maxSize + 1);
	h->maxSize = maxSize;
	h->size = maxSize;

	int i;
	for (i = 1; i <= maxSize; i++)
	{
		h->elements[i] = elements[i];
		h->elements[i]->hIndex = i;
	}

	for (i = maxSize / 2; i >= 1; i--) heapifyMaxHeap(h, i);
	return h;
}

void deleteMaxHeapFromArray(MaxHeap h)
{
	int i;
	for (i = 1; i <= h->size; i++)
	{
		if (h->elements[i] != NULL) free(h->elements[i]);
	}

	free(h->elements);
	free(h);
}


Node extractMax(MaxHeap h)
{
	Node toBeReturned = h->elements[1];
	h->elements[1] = h->elements[h->size];
	h->elements[1]->hIndex = 1;
	h->size = h->size - 1;
	heapifyMaxHeap(h, 1);

	return toBeReturned;
}

void insertMaxHeap(MaxHeap h, double key, int data)
{
	if (h->size >= h->maxSize)
		return;

	h->size = h->size + 1;
	int i = h->size;

	while (i > 1 && h->elements[i / 2]->key < key)
	{
		h->elements[i] = h->elements[i / 2];
		h->elements[i]->hIndex = i;
		i = i / 2;
	}

	Node newElement = malloc(sizeof(struct Node)); if (newElement == NULL) return;
	newElement->key = key;
	newElement->data = data;
	newElement->hIndex = i;
	h->elements[i] = newElement;
}

//TODO
void increaseKeyMaxHeap(MaxHeap h, int i, double key)
{
	if (key < h->elements[i]->key)
		return;

	Node temp = h->elements[i];

	int iOver2 = i / 2;
	while (i > 1 && h->elements[iOver2]->key < key)
	{
		h->elements[i] = h->elements[iOver2];
		h->elements[i]->hIndex = i;
		i = iOver2;
		iOver2 = i / 2;
	}

	h->elements[i] = temp;
	h->elements[i]->key = key;
	h->elements[i]->hIndex = i;
}

/* Not used currently
void decreaseKey(MaxHeap h, int i, int key)
{
	if (key > h->elements[i]->key)
		return;

	h->elements[i]->key = key;
	heapify(h, i);
}
*/

void heapifyMaxHeap(MaxHeap h, int i)
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
		heapifyMaxHeap(h, largest);
	}
}

int sizeMaxHeap(MaxHeap h)
{
	return h->size;
}

void printMaxHeap(MaxHeap h)
{
	int i;
	for (i = 1; i <= h->size; i++)
	{
		printf("At index %d, the node is {key=%f, data=%d, hIndex=%d}\n", i, h->elements[i]->key, h->elements[i]->data, h->elements[i]->hIndex);
	}
}