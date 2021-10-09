#include "Stack.h"

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 5
*/

Stack::Stack()
{
}

int Stack::size() const
{
	return list.size();
}

bool Stack::isEmpty()
{
	return (size() == 0);
}

int Stack::pop()
{
	if (size() == 0)
		return -1;
	else
	{
		int item;
		list.deleteHead(item);
		return item;
	}
}

void Stack::push(const int item)
{
	list.add(item);
}
