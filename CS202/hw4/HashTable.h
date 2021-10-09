#pragma once

enum CollisionStrategy { LINEAR, QUADRATIC, DOUBLE };
enum State {OCCUPIED, EMPTY, DELETED};

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 4
*/

class HashTable
{
public:
	HashTable(const int tableSize, const CollisionStrategy option);
	~HashTable();
	bool insert(const int item);
	bool remove(const int item);
	bool search(const int item, int& numProbes);
	void display();
	void analyze(double& numSuccProbes, double& numUnsuccProbes);

private:
	int hash(int key, int i);
	int reverse(int key);

	int* table;
	State* states;
	int tableSize;
	CollisionStrategy option;
};

