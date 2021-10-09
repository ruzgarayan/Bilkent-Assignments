#pragma once

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

List initList();
void deleteList(List l);
void insertFirst(List l, int data);
int deleteFirst(List l);
int getSize(List l);