#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include "MaxHeap.h"
#include "List.h"
#include <float.h>

int* vertices;
int* edges;
double** weights;
int V;
int E;
double* d;

//Parses the ".mtx" files.
void parseMTX(char fileName[])
{
	FILE* file;
	file = fopen(fileName, "r");
	if (file == 0)
		return;

	List* adjList = NULL;

	int src, dst;
	double wght;

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
			sscanf(linebuf, "%d%d%d", &V, &V, &E);

			//Initialize required data lists and arrays for parsing and storing the graph.
			adjList = (List*)malloc(sizeof(List) * (V + 1)); if (adjList == NULL) return;
			for (i = 0; i <= V; i++) adjList[i] = initList();

			edges = (int*)malloc(sizeof(int) * E); if (edges == NULL) return;
			vertices = (int*)malloc(sizeof(int) * (V + 2)); if (vertices == NULL) return;
			weights = malloc(sizeof(double*) * (V + 1)); if (weights == NULL) return;
			for (i = 0; i <= V; i++)
			{
				weights[i] = malloc(sizeof(double) * (V + 1)); if (weights[i] == NULL) return;
			}
			vertices[V + 1] = E;

			line++;
		}
		else
		{
			sscanf(linebuf, "%d%d%lf", &src, &dst, &wght);
			insertFirst(adjList[src], dst);
			weights[src][dst] = wght;
		}
	}
	fclose(file);

	int listSize;
	int currentVertex;
	int j;
	i = 0;
	for (currentVertex = 0; currentVertex <= V; currentVertex++)
	{
		vertices[currentVertex] = i;
		listSize = getSize(adjList[currentVertex]);
		for (j = 0; j < listSize; j++)
		{
			edges[i] = deleteFirst(adjList[currentVertex]);
			i++;
		}
	}

	for (i = 0; i <= V; i++) deleteList(adjList[i]);
	if (adjList != NULL) free(adjList);
}

void initializeSingleSourceZero(int source)
{
	d = (double*)malloc(sizeof(double) * (V + 1)); if (d == NULL) return;
	int i;
	for (i = 1; i <= V; i++)
	{
		d[i] = -1;
	}
	d[source] = 1;
}

void dijkstraB(int source)
{
	initializeSingleSourceZero(source);
	int i, j;

	Node* heapNodes = malloc(sizeof(Node) * (V + 1)); if (heapNodes == NULL) return;
	for (i = 1; i <= V; i++) {
		Node heapNode = malloc(sizeof(struct Node)); if (heapNode == NULL) return;
		heapNode = heapNode;
		heapNode->key = d[i];
		heapNode->data = i;
		heapNode->hIndex = i;
		heapNodes[i] = heapNode;
	}
	MaxHeap Q = initMaxHeapFromArray(heapNodes, V);
	//printMaxHeap(Q);
	while (sizeMaxHeap(Q) != 0)
	{
		Node uNode = extractMax(Q);
		int u = uNode->data;
		for (i = vertices[u]; i < vertices[u + 1]; i++)
		{
			int v = edges[i];
			//Below is relax
			if (d[v] < d[u] * weights[u][v])
			{
				d[v] = d[u] * weights[u][v];
				increaseKeyMaxHeap(Q, heapNodes[v]->hIndex, d[v]);
			}
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2)
	{
		printf("Wrong arguments\n");
		return -1;
	}
	int i;

	parseMTX(argv[1]);
	dijkstraB(1);

	FILE* file = fopen("b.txt", "w");
	if (file == 0)
		return;

	for (i = 1; i <= V; i++)
	{
		if (d[i] > DBL_MAX / 10 || d[i] < 0)
			fprintf(file, "-1\n");
		else
			fprintf(file, "%.8f\n", d[i]);
	}
	fclose(file);

	free(vertices);
	free(edges);
	free(d);
	for (i = 0; i <= V; i++)
	{
		free(weights[i]);
	}
	free(weights);
	return 0;
}