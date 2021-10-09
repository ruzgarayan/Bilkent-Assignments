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

void insertFirst(List l, const struct BurstInfo data)
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

void insertLast(List l, const struct BurstInfo data)
{	
	ListNode newNode = malloc(sizeof(struct ListNode));
	newNode->data = data;

	int size = l->size;
	
	if (size == 0)
	{
		insertFirst(l, data);
		return;
	}

	ListNode curr = l->head;
	for (int i = 0; i < size - 1; i++)
		curr = curr->next;
	curr->next = newNode;

	l->size = l->size + 1;
}

struct BurstInfo get(List l, int index)
{
	if (l->size <= index)
	{
		struct BurstInfo emptyBurstInfo;
		emptyBurstInfo.threadIndex = -1;
		emptyBurstInfo.burstIndex = -1;
		emptyBurstInfo.length = -1;
		return emptyBurstInfo;
	}

	ListNode curr = l->head;
	for (int i = 0; i < index; i++)
		curr = curr->next;
	return curr->data;
}

struct BurstInfo deleteFirst(List l)
{
	if (l->size == 0)
	{
		struct BurstInfo emptyBurstInfo;
		emptyBurstInfo.threadIndex = -1;
		emptyBurstInfo.burstIndex = -1;
		emptyBurstInfo.length = -1;
		return emptyBurstInfo;
	}

	struct BurstInfo data = l->head->data;
	ListNode head = l->head;
	l->head = head->next;
	free(head);
	l->size = l->size - 1;

	return data;
}

struct BurstInfo deleteAtIndex(List l, int index){
	if (l->size <= index)
	{
		struct BurstInfo emptyBurstInfo;
		emptyBurstInfo.threadIndex = -1;
		emptyBurstInfo.burstIndex = -1;
		emptyBurstInfo.length = -1;
		return emptyBurstInfo;
	}
	if (index == 0)
	{
		return deleteFirst(l);
	}
	
	ListNode curr = l->head;
	for (int i = 0; i < index - 1; i++)
		curr = curr->next;
	
	ListNode nodeToDelete = curr->next;
	struct BurstInfo data = nodeToDelete->data;
	curr->next = curr->next->next;
	free(nodeToDelete);
	l->size = l->size - 1;

	return data;
}

int getSize(const List l)
{
	return l->size;
}


int empty(const List l)
{
	return (l->size == 0);
}