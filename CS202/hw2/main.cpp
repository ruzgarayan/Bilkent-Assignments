#include "DecisionTree.h"
#include <iostream>

/**
* Author : RüzgarAyan
* ID: 21801984
* Section : 2
* Assignment : 2
*/

int main()
{
	DecisionTree dt;
	cout << "Training the decision tree with the file \"train_data.txt\"" << endl;
	dt.train("train_data.txt", 498, 21);
	cout << "Training complete." << endl;
	cout << "Preorder print of the decision tree:" << endl << endl;
	dt.print();
	cout << endl << endl << "Testing with the file \"test_data.txt\"" << endl;
	double testResult = dt.test("test_data.txt", 473);
	cout <<"Test accuracy:" << testResult << endl;
	return 0;
}