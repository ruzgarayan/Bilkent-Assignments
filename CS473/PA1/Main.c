#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include "MaxHeap.h"
#include "LinkedList.h"
#include <stdlib.h>
#include <time.h>

//Returns 1 if there is an edge between a and b, returns 0 otherwise
int edge(int* vertices, int* edges, int a, int b)
{
	int i;
	for (i = vertices[a]; i < vertices[a + 1]; i++)
	{
		if (edges[i] == b)
			return 1;
	}
	return 0;
}

//Computes the cutsize according to the given partition
int computeCutSize(int* vertices, int* edges, int v, int e, int* partition)
{
	int i, j;
	int cutSize = 0;
	for (i = 0; i < v; i++)
	{
		for (j = vertices[i]; j < vertices[i + 1]; j++)
		{
			if (partition[i] != partition[edges[j]])
			{
				cutSize++;
			}
		}
	}

	return cutSize / 2;
}

//Computes the d value for a given vertex.
int computeD(int* vertices, int* edges, int v, int e, int* partition, int vertex)
{
	int i;
	int d = 0;
	for (i = vertices[vertex]; i < vertices[vertex + 1]; i++)
	{
		if (partition[edges[i]] == partition[vertex]) d--;
		else d++;
	}

	return d;
}

//Finds the a and b vertices that maximize the D values according to the partition
void findMaximizingAB(int* vertices, int* edges, int v, int e, int* partition, int* D, int* locked, int* a, int* b)
{
	int i;
	int maxA = -v; int maxB = -v;
	int maxAIndex = 0; int maxBIndex = 0;
	for (i = 0; i < v; i++)
	{
		if (D[i] > maxA && locked[i] == 0 && partition[i] == 0)
		{
			maxA = D[i];
			maxAIndex = i;
		}
		else if (D[i] > maxB && locked[i] == 0 && partition[i] == 1)
		{
			maxB = D[i];
			maxBIndex = i;
		}
	}

	(*a) = maxAIndex;
	(*b) = maxBIndex;
}

//Parses the ".mtx" files and returns the results with the last 4 parameters.
void parseMTX(char fileName[], int** vertices_, int** edges_, int* vCount, int* eCount)
{
	FILE* file;
	file = fopen(fileName, "r");
	if (file == 0)
		return;

	int v;
	int e;
	List* adjList = NULL;

	int src, dst;

	char linebuf[1000];
	int line = 0;
	int i;
	while (fgets(linebuf, sizeof linebuf, file) != NULL)
	{
		if (linebuf[0] == '%')
		{
			continue;
		}
		if (line == 0)
		{
			sscanf(linebuf, "%d%d%d", &v, &v, &e);
			adjList = (List*) malloc(sizeof(List) * v); if (adjList == NULL) return;
			for (i = 0; i < v; i++) adjList[i] = initList();

			line++;
		}
		else
		{ 
			sscanf(linebuf, "%d%d", &src, &dst);
			src--; dst--; //Make the vertices 0-indexed.
			insertFirst(adjList[src], dst);
			insertFirst(adjList[dst], src);
		}
	}
	fclose(file);

	int* edges = (int*)malloc(sizeof(int) * e * 2); if (edges == NULL) return;
	int* vertices = (int*)malloc(sizeof(int) * (v + 1)); if (vertices == NULL) return;
	vertices[v] = 2 * e;

	int listSize;
	int currentVertex;
	int j;
	i = 0;
	for (currentVertex = 0; currentVertex < v; currentVertex++)
	{
		vertices[currentVertex] = i;
		listSize = getSize(adjList[currentVertex]);
		for (j = 0; j < listSize; j++)
		{
			edges[i] = deleteFirst(adjList[currentVertex]);
			i++;
		}
	}

	(*vCount) = v;
	(*eCount) = e;
	(*vertices_) = vertices;
	(*edges_) = edges;

	for (i = 0; i < v; i++) deleteList(adjList[i]);
	if(adjList!=NULL) free(adjList);
}

//Implementation for 1.b)
void KL(int* vertices, int* edges, int v, int e, int* initialCutSize, int* finalCutSize)
{
	int i, j;

	//Partition is 0 for a, 1 for b 
	int* partition = (int*)malloc(sizeof(int) * v); if (partition == NULL) return;
	for (i = 0; i < v; i++)
	{
		if (i < (v / 2)) partition[i] = 0;
		else partition[i] = 1;
	}

	(*initialCutSize) = computeCutSize(vertices, edges, v, e, partition);

	int* D = (int*)malloc(sizeof(int) * v); if (D == NULL) return;
	int* locked = (int*)malloc(sizeof(int) * v); if (locked == NULL) return;
	int* L_g = (int*)malloc(sizeof(int) * (v / 2)); if (L_g == NULL) return;
	int* L_a = (int*)malloc(sizeof(int) * (v / 2)); if (L_a == NULL) return;
	int* L_b = (int*)malloc(sizeof(int) * (v / 2)); if (L_b == NULL) return;
	int* prefixSum = (int*)malloc(sizeof(int) * v); if (prefixSum == NULL) return;

	int maxSwap;
	do
	{
		for (i = 0; i < v; i++)
		{
			D[i] = computeD(vertices, edges, v, e, partition, i);
		}
		//1 for locked, 0 for not locked
		for (i = 0; i < v; i++) locked[i] = 0;

		int aIndex = 0, bIndex = 0;
		int lastMaxA, lastMaxB;
		int lastMaxAIndex = -1, lastMaxBIndex = -1;
		for (i = 0; i < (v / 2); i++)
		{
			if (lastMaxAIndex == -1 || lastMaxBIndex == -1) //Improvement that I've made, we dont come here every time.
			{
				findMaximizingAB(vertices, edges, v, e, partition, D, locked, &aIndex, &bIndex);
			}
			else
			{
				aIndex = lastMaxAIndex;
				bIndex = lastMaxBIndex;
			}
			lastMaxA = D[aIndex]; lastMaxB = D[bIndex];
			lastMaxAIndex = -1; lastMaxBIndex = -1;

			//Store the results in the L arrays
			L_a[i] = aIndex;
			L_b[i] = bIndex;
			L_g[i] = D[aIndex] + D[bIndex];
			if (edge(vertices, edges, aIndex, bIndex)) L_g[i] = L_g[i] - 2;

			locked[aIndex] = 1;
			locked[bIndex] = 1;

			//Update d values
			for (j = vertices[aIndex]; j < vertices[aIndex + 1]; j++)
			{
				if (locked[edges[j]]) continue;

				if (partition[edges[j]] == 0)
				{
					D[edges[j]] += 2;
					if (D[edges[j]] >= lastMaxA)
					{
						lastMaxA = D[edges[j]];
						lastMaxAIndex = edges[j];
					}
				}
				else D[edges[j]] -= 2;
			}
			for (j = vertices[bIndex]; j < vertices[bIndex + 1]; j++)
			{
				if (locked[edges[j]]) continue;

				if (partition[edges[j]] == 1)
				{
					D[edges[j]] += 2;
					if (D[edges[j]] >= lastMaxB)
					{
						lastMaxB = D[edges[j]];
						lastMaxBIndex = edges[j];
					}
				}
				else
				{
					if (edges[j] == lastMaxAIndex)
						lastMaxAIndex = -1;
					D[edges[j]] -= 2;
				}
			}

		}

		//Compute prefix sums efficiently
		prefixSum[0] = L_g[0];
		for (i = 1; i < (v / 2); i++) prefixSum[i] = L_g[i] + prefixSum[i - 1];

		//Find the best k value given in the pseudo-code
		maxSwap = 0;
		int maxSwapIndex = -1;
		for (i = 0; i < (v / 2); i++)
		{
			if (prefixSum[i] > maxSwap)
			{
				maxSwap = prefixSum[i];
				maxSwapIndex = i;
			}
		}

		//Make the swaps 
		for (i = 0; i <= maxSwapIndex; i++)
		{
			partition[L_a[i]] = 1;
			partition[L_b[i]] = 0;
		}

	} while (maxSwap > 0);

	(*finalCutSize) = computeCutSize(vertices, edges, v, e, partition);

	free(partition);
	free(D);
	free(locked);
	free(L_g);
	free(L_a);
	free(L_b);
	free(prefixSum);
}

//Implementation for 1.a)
void KLWithHeap(int* vertices, int* edges, int v, int e, int* initialCutSize, int* finalCutSize)
{
	int i, j;

	//Partition is 0 for a, 1 for b 
	int* partition = (int*)malloc(sizeof(int) * v); if (partition == NULL) return;
	int aSize = (v / 2);
	int bSize = v - aSize;

	for (i = 0; i < v; i++)
	{
		if (i < (v / 2)) partition[i] = 0;
		else partition[i] = 1;
	}

	(*initialCutSize) = computeCutSize(vertices, edges, v, e, partition);

	int* D = (int*)malloc(sizeof(int) * v); if (D == NULL) return;
	int* locked = (int*)malloc(sizeof(int) * v); if (locked == NULL) return;
	int* L_g = (int*)malloc(sizeof(int) * (v / 2)); if (L_g == NULL) return;
	int* L_a = (int*)malloc(sizeof(int) * (v / 2)); if (L_a == NULL) return;
	int* L_b = (int*)malloc(sizeof(int) * (v / 2)); if (L_b == NULL) return;
	int* prefixSum = (int*)malloc(sizeof(int) * v); if (prefixSum == NULL) return;

	Node* heapNodes = malloc(sizeof(Node) * v); if (heapNodes == NULL) return; for (i = 0; i < v; i++) { Node heapNode = malloc(sizeof(struct Node)); heapNodes[i] = heapNode; }
	Node* nodesA = malloc(sizeof(Node) * aSize); if (nodesA == NULL) return; for (i = 0; i < aSize; i++) { Node node = malloc(sizeof(struct Node)); nodesA[i] = node; }
	Node* nodesB = malloc(sizeof(Node) * bSize); if (nodesB == NULL) return; for (i = 0; i < bSize; i++) { Node node = malloc(sizeof(struct Node)); nodesB[i] = node; }

	int maxSwap;
	do
	{
		for (i = 0; i < v; i++)
		{
			D[i] = computeD(vertices, edges, v, e, partition, i);
		}

		//Original array that holds all the vertices.
		for (i = 0; i < v; i++)
		{
			heapNodes[i]->key = D[i];
			heapNodes[i]->data = i;
			heapNodes[i]->hIndex = i + 1;
		}
		int a = 0, b = 0;
		//A and B arrays that point to the vertices in the original array.
		for (i = 0; i < v; i++)
		{
			if (partition[i] == 0) nodesA[a++] = heapNodes[i];
			else nodesB[b++] = heapNodes[i];
		}
		//Build heaps from these two.
		MaxHeap heapA = initHeapFromArray(nodesA, aSize);
		MaxHeap heapB = initHeapFromArray(nodesB, bSize);

		//1 for locked, 0 for not locked
		for (i = 0; i < v; i++) locked[i] = 0;

		int aIndex = 0, bIndex = 0;
		for (i = 0; i < (v / 2); i++)
		{
			aIndex = extractMax(heapA);
			bIndex = extractMax(heapB);

			//Store the results in the L arrays
			L_a[i] = aIndex;
			L_b[i] = bIndex;
			L_g[i] = heapNodes[aIndex]->key + heapNodes[bIndex]->key;
			if (edge(vertices, edges, aIndex, bIndex)) L_g[i] = L_g[i] - 2;

			locked[aIndex] = 1;
			locked[bIndex] = 1;

			//Update d values
			for (j = vertices[aIndex]; j < vertices[aIndex + 1]; j++)
			{
				if (locked[edges[j]]) continue;

				int hIndex = heapNodes[edges[j]]->hIndex;
				int currentD = heapNodes[edges[j]]->key;

				if (partition[edges[j]] == 0) increaseKey(heapA, hIndex, currentD + 2);
				else decreaseKey(heapB, hIndex, currentD - 2);
			}
			for (j = vertices[bIndex]; j < vertices[bIndex + 1]; j++)
			{
				if (locked[edges[j]]) continue;

				int hIndex = heapNodes[edges[j]]->hIndex;
				int currentD = heapNodes[edges[j]]->key;

				if (partition[edges[j]] == 1) increaseKey(heapB, hIndex, currentD + 2);
				else decreaseKey(heapA, hIndex, currentD - 2);
			}

		}

		//Compute prefix sums efficiently
		prefixSum[0] = L_g[0];
		for (i = 1; i < (v / 2); i++) prefixSum[i] = L_g[i] + prefixSum[i - 1];

		//Find the best k value given in the pseudo-code
		maxSwap = 0;
		int maxSwapIndex = -1;
		for (i = 0; i < (v / 2); i++)
		{
			if (prefixSum[i] > maxSwap)
			{
				maxSwap = prefixSum[i];
				maxSwapIndex = i;
			}
		}

		//Make the swaps
		for (i = 0; i <= maxSwapIndex; i++)
		{
			partition[L_a[i]] = 1;
			partition[L_b[i]] = 0;
		}

		//printf("\nFinal cut size is %d\n", computeCutSize(vertices, edges, v, e, partition));

		deleteHeapFromArray(heapA);
		deleteHeapFromArray(heapB);

	} while (maxSwap > 0);

	(*finalCutSize) = computeCutSize(vertices, edges, v, e, partition);

	free(partition);
	free(D);
	free(locked);
	free(L_g);
	free(L_a);
	free(L_b);
	free(prefixSum);
	free(heapNodes);
	free(nodesA);
	free(nodesB);
}

int main(int argc, char* argv[]) {
	if (argc != 3)
	{
		printf("Wrong arguments\n");
		return -1;
	}

	int* vertices = NULL;
	int* edges = NULL;
	int vCount = 0, eCount = 0;
	parseMTX(argv[1], &vertices, &edges, &vCount, &eCount);

	int initialCutSize = 0;
	int finalCutSize = 0;

	clock_t t;
	t = clock();

	if (argv[2][0] == 'a')
		KLWithHeap(vertices, edges, vCount, eCount, &initialCutSize, &finalCutSize);
	else if (argv[2][0] == 'b')
		KL(vertices, edges, vCount, eCount, &initialCutSize, &finalCutSize);

	t = clock() - t;
	double time_taken1 = ((double)t) / CLOCKS_PER_SEC;
	printf("%d %d %.1fs\n", initialCutSize, finalCutSize, time_taken1);

	free(vertices);
	free(edges);
	return 0;
}
