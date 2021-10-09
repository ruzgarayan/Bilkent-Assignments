#include <string>
#include "SimpleLinkedList.h"
using namespace std;

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 5
*/

class FriendNet {
public:
	FriendNet(); // empty constructor
	FriendNet(const string fileName); // constructor
	FriendNet(const FriendNet& aNet); // copy constructor
	~FriendNet(); // destructor
	void listFriends(const string personName, const int hopNo);
	void displayAverageDegrees();
	void displayDiameters();
private:
	SimpleLinkedList* getConnectedComponents(int& length);
	int singleSourceLongestDistance(int startingNode);

	SimpleLinkedList* adjList;
	string* names;
	int numOfV;
};