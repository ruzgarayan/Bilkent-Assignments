#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

struct ListNode {
	int data;
	struct ListNode* next;
};
typedef struct ListNode* ListNode;

struct List {
	ListNode head;
	int size;
};
typedef struct List* List;

List initList()
{
	List l;
	l = malloc(sizeof(struct List));
	ListNode head = NULL;
	l->head = head;
	l->size = 0;

	return l;
}

void insertFirst(List l, int data)
{
	ListNode newNode = malloc(sizeof(struct ListNode));
	newNode->data = data;

	ListNode oldHead = l->head;
	l->head = newNode;

	l->size = l->size + 1;
	if (oldHead == NULL)
	{
		newNode->next = NULL;
		return;
	}
	newNode->next = oldHead;
}


int deleteFirst(List l)
{
	if (l->size == 0)
	{
		return -1;
	}

	int data = l->head->data;
	ListNode head = l->head;
	l->head = head->next;
	free(head);
	l->size = l->size - 1;

	return data;
}

void deleteList(List l)
{
	while (l->size > 0)
	{
		deleteFirst(l);
	}
	free(l);
}

int getSize(List l)
{
	return l->size;
}

int main()
{
	struct timeval start_time;
	gettimeofday(&start_time, NULL);
	int ADD_CNT = 10000;
	srand(time(NULL));
	
	List list = initList();
	
	int i;
	for (i = 0; i < ADD_CNT; i++)
	{
		insertFirst(list, rand());
	}
	
	deleteList(list);
	
	struct timeval end_time;
	gettimeofday(&end_time, NULL);
	long elapsedMicroSeconds = (end_time.tv_usec - start_time.tv_usec);
	printf("%lu\n", elapsedMicroSeconds);
	
	return 0;
}
