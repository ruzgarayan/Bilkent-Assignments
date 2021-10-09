#include "Queue.h"

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 5
*/

Queue::Queue()
{
}

int Queue::size() const
{
	return list.size();
}

bool Queue::isEmpty()
{
	return (size() == 0);
}

int Queue::dequeue()
{
	if (isEmpty())
		return -1;
	else
	{
		int dequeuedItem;
		list.deleteLast(dequeuedItem);
		return dequeuedItem;
	}
}

void Queue::enqueue(const int item)
{
	list.add(item);
}
