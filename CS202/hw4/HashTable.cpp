#include "HashTable.h"
#include <iostream>

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 4
*/

HashTable::HashTable(const int tableSize, const CollisionStrategy option)
{
	this->tableSize = tableSize;
	this->option = option;

	table = new int[tableSize];
	states = new State[tableSize];
	//All cells are initially empty.
	for (int i = 0; i < tableSize; i++)
		states[i] = EMPTY;
}

HashTable::~HashTable()
{
	delete[] table;
	delete[] states;
}

bool HashTable::insert(const int item)
{
	int i = 0;
	//Probe until a non-OCCUPIED cell is found, until tableSize probes.
	while (states[hash(item, i)] == OCCUPIED && i < tableSize)
		i++;

	//If we left the loop because of this condition, then return false.
	if (i == tableSize)
		return false;

	//Otherwise insert
	int insertLocation = hash(item, i);
	states[insertLocation] = OCCUPIED;
	table[insertLocation] = item;

	return true;
}

bool HashTable::remove(const int item)
{
	int i = 0;
	//Probe until finding an empty cell or probing tableSize times.
	while (states[hash(item, i)] != EMPTY && i < tableSize)
	{
		//If we find a OCCUPIED cell with the target item, delete it and return.
		if (states[hash(item, i)] == OCCUPIED && table[hash(item, i)] == item)
		{
			states[hash(item, i)] = DELETED;
			table[hash(item, i)] = 0; //To guarantee
			return true;
		}
		i++;
	}

	//If we have left the loop without deleting, return false.
	return false;
}

bool HashTable::search(const int item, int& numProbes)
{
	int i = 0;
	//Probe until finding an empty cell or probing tableSize times.
	while (states[hash(item, i)] != EMPTY && i < tableSize)
	{
		//If we find a OCCUPIED cell with the target item, we have found our item and we can return true.
		if (states[hash(item, i)] == OCCUPIED && table[hash(item, i)] == item)
		{
			numProbes = i + 1;
			return true;
		}
		i++;
	}
	//If we have left the loop without finding the target item,
	//we will return false and there are 2 cases.

	//If we have left the loop because we probed tableSize times,
	//set numProbes to tableSize before returning.
	if (i == tableSize)
	{
		numProbes = tableSize;
		return false;
	}

	//Otherwise set the numProbes to i + 1.
	numProbes = i + 1;
	return false;

	
}

void HashTable::display()
{
	for (int i = 0; i < tableSize; i++)
	{
		//Only display the item in table if that cell is OCCUPIED
		if (states[i] == OCCUPIED)
			std::cout << i << ": " << table[i] << std::endl;
		else
			std::cout << i << ": " << std::endl;
	}
}

void HashTable::analyze(double& numSuccProbes, double& numUnsuccProbes)
{
	int sum = 0;
	int numProbes;
	int currentSize = 0;
	for (int i = 0; i < tableSize; i++)
	{
		//Call search function for each item in the OCCUPIED cells.
		if (states[i] == OCCUPIED)
		{
			search(table[i], numProbes);
			sum += numProbes;
			currentSize++;
		}
	}
	numSuccProbes = (sum + 0.0) / currentSize;

	//If the collision strategy is DOUBLE, we don't do anything for unsuccessful searchs and just return.
	if (option == DOUBLE)
	{
		numUnsuccProbes = -1;
		return;
	}

	sum = 0;
	for (int i = 0; i < tableSize; i++)
	{
		numProbes = 0;
		//Find how many probes is required to find an empty cell here.
		while (states[hash(i, numProbes)] != EMPTY && numProbes < tableSize)
			numProbes++;
		if (numProbes == tableSize)
			sum += numProbes;
		else
			sum += numProbes + 1;
	}
	numUnsuccProbes = (sum + 0.0) / tableSize;
}

//Helper function that returns the i'th probe for the given key
int HashTable::hash(int key, int i)
{
	int collisionFunction = 0;

	//According to the chosen collision strategy, 
	//compute the number f(i), that will be added to the key.
	switch (option) {
	case LINEAR:
		collisionFunction = i;
		break;
	case QUADRATIC:
		collisionFunction = i * i;
		break;
	case DOUBLE:
		collisionFunction = i * reverse(key);
		break;
	default:
		break;
	}
	int hash = key % tableSize;

	return (hash + collisionFunction) % tableSize;
}

//Helper function that reverses an integer.
int HashTable::reverse(int key)
{
	bool negative = false;
	if (key < 0)
	{
		negative = true;
		key = -key;
	}

	int reversed = 0;
	while (key > 0)
	{
		reversed = 10 * reversed + (key % 10);
		key /= 10;
	}
	
	return negative ? -reversed : reversed;
}