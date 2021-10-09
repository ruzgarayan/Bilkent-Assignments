#include <fstream>
#include <iostream>
#include "HashTable.h"

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 4
*/

using namespace std;
int main()
{
	const int tableSize = 13;
	const CollisionStrategy cs = LINEAR;

	ifstream in("test.txt");
	HashTable table(tableSize, cs);

	char op;
	int num;
	bool success;
	int numProbes;

	do {
		in >> op;
		in >> num; 
		//cout << op << " " << num << endl;

		if (op == 'I')
		{
			success = table.insert(num);
			if (success)
				cout << num << " inserted" << endl;
			else
				cout << num << " not inserted" << endl;
		}
		else if (op == 'R')
		{
			success = table.remove(num);
			if (success)
				cout << num << " removed" << endl;
			else
				cout << num << " not removed" << endl;
		}
		else if (op == 'S')
		{
			success = table.search(num, numProbes);
			if (success)
				cout << num << " found after " << numProbes << " probes" << endl;
			else
				cout << num << " not found after " << numProbes << " probes" << endl;
		}

	} while (!in.eof());

	cout << endl << endl << "The size of the table used is " << tableSize << "." << endl;

	table.display();
	double numSuccProbes, numUnsuccProbes;
	table.analyze(numSuccProbes, numUnsuccProbes);
	cout << "Average number of probes successful searches: " << numSuccProbes << endl;
	cout << "Average number of probes unsuccessful searches: " << numUnsuccProbes << endl;

	return 0;
}