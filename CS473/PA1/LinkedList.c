#include "LinkedList.h"
#include <stdlib.h>
#include <stdio.h>

List initList()
{
	List l;
	l = malloc(sizeof(struct List));
	ListNode head = NULL;
	l->head = head;
	l->size = 0;

	return l;
}

void deleteList(List l)
{
	while (l->size > 0)
	{
		deleteFirst(l);
	}
	free(l);
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

int getSize(List l)
{
	return l->size;
}