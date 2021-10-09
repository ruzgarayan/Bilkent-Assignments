#pragma once

#include <sys/time.h>

struct BurstInfo{
	int threadIndex;
	int burstIndex;
	int length;
	struct timeval generationTime;
};

struct ListNode {
	struct BurstInfo data;
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
void insertFirst(List l, const struct BurstInfo data);
void insertLast(List l, const struct BurstInfo data);
struct BurstInfo get(List l, int index);
struct BurstInfo deleteFirst(List l);
struct BurstInfo deleteAtIndex(List l, int index);
int getSize(const List l);
int empty(const List l);