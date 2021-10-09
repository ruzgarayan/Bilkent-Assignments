#include "FriendNet.h"
#include <fstream>
#include <iostream>
#include "Stack.h" 
#include <iomanip>
#include "Queue.h"

using namespace std;

/**
* Author : Ruzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 5
*/

//Default constructor
FriendNet::FriendNet()
{
	//Create an empty graph
	numOfV = 0;
	adjList = new SimpleLinkedList[numOfV];
	names = new string[numOfV];
}

//Constructor that reads the data from file.
FriendNet::FriendNet(const string fileName)
{
	//Create input stream from the file
	ifstream in(fileName);

	//If file does not exist, create an empty graph.
	if (!in.good())
	{
		numOfV = 0;
		adjList = new SimpleLinkedList[numOfV];
		names = new string[numOfV];
	}
	//If the file exists, parse the data.
	else
	{
		in >> numOfV;
		adjList = new SimpleLinkedList[numOfV];
		names = new string[numOfV];
		int id, degree, friend_id;
		for (int i = 0; i < numOfV; i++)
		{
			in >> id;
			in >> names[id];
			in >> degree;
			for (int j = 0; j < degree; j++)
			{
				in >> friend_id;
				adjList[id].add(friend_id);
			}
		}
	}
}

//copy constructor	
FriendNet::FriendNet(const FriendNet& aNet)
{
	numOfV = aNet.numOfV;
	adjList = new SimpleLinkedList[numOfV];
	names = new string[numOfV];

	for (int i = 0; i < numOfV; i++)
	{
		adjList[i] = aNet.adjList[i]; //SimpleLinkedList has overloaded assignment operator.
		names[i] = aNet.names[i];
	}
}

FriendNet::~FriendNet()
{
	delete[] adjList;
	delete[] names;
}

void FriendNet::listFriends(const string personName, const int hopNo)
{
	//We first find the id of the person with the given personName.
	int idOfPerson = -1;
	for (int i = 0; i < numOfV; i++)
	{
		if (names[i] == personName)
		{
				idOfPerson = i;
				break;
		}
	}
	//If we haven't found the name, then print message and return.
	if (idOfPerson == -1)
	{
		cout << personName << " does not exist in the network." << endl;
		return;
	}
	
	bool* reachedSoFar = new bool[numOfV]; //The nodes in the graph that are reached so far.
	bool* currentHop = new bool[numOfV]; //The nodes that are reached with ith hop.
	bool* nextHop = new bool[numOfV]; //The nodes that are reached with (i+1)th hop.
	//Initially all false except currentHop[idOfPerson] (starting node)
	for (int i = 0; i < numOfV; i++)
	{
		reachedSoFar[i] = false;
		currentHop[i] = false;
		nextHop[i] = false;
	}
	currentHop[idOfPerson] = true;

	bool nobodyAccessible = true; //If this is an isolated node, then this is true.
	//Make hopNo number of hops.
	for (int i = 0; i < hopNo; i++)
	{
		for (int j = 0; j < numOfV; j++)
		{
			//For every node in the currentHop, do the following:
			if (currentHop[j])
			{
				//Get its neighbors from the adjList.
				int noOfNeighbors;
				int* neighbors = adjList[j].getListAsArray(noOfNeighbors);

				//For each of that node's neighbors, do the following:
				for (int k = 0; k < noOfNeighbors; k++)
				{
					//If that neighbor is different from the starting node,
					if (neighbors[k] != idOfPerson)
					{
						//We can access other nodes, therefore not isolated.
						nobodyAccessible = false;
						//Will get into this node in the next hop.
						nextHop[neighbors[k]] = true;
						//We are able to reach this node in hopNo hops.
						reachedSoFar[neighbors[k]] = true;
					}
				}

				delete[] neighbors;
			}
		}

		//Shift currentHop into nextHop and reset nextHop to all false.
		for (int j = 0; j < numOfV; j++)
		{
			currentHop[j] = nextHop[j];
			nextHop[j] = false;
		}
	}

	//Print the obtained information.
	cout << "People accessible from " << personName << " within " << hopNo << " hops: ";
	if (nobodyAccessible)
		cout << "NOBODY" << endl;
	else
	{
		bool firstName = true; //To put commas before the nodes except the first node.
		for (int i = 0; i < numOfV; i++)
		{
			//Print the names that can be reached.
			if (reachedSoFar[i])
			{
				if (firstName)
				{
					cout << names[i];
					firstName = false;
				}
				else
				{
					cout << ", " << names[i];
				}
			}
		}
		cout << endl;
	}

	//delete dynamically allocated arrays.
	delete[] reachedSoFar;
	delete[] currentHop;
	delete[] nextHop;
}

void FriendNet::displayAverageDegrees()
{
	//Get the connected components using the getConnectedComponents() function
	int lengthConnComponents;
	SimpleLinkedList* connComponents = getConnectedComponents(lengthConnComponents);

	//Print the first sentence of the information.
	cout << "There are " << lengthConnComponents << " connected components. The average degrees are:" << endl;

	int sum;

	//For each connected component:
	for (int i = 0; i < lengthConnComponents; i++)
	{
		//Get the nodes in that connected components as an array.
		int sizeOfComponent;
		int* nodesInComponent = connComponents[i].getListAsArray(sizeOfComponent);

		//Sum the degrees of the nodes in that connected component.
		sum = 0;
		for (int j = 0; j < sizeOfComponent; j++)
		{
			sum += adjList[nodesInComponent[j]].size();
		}

		//Print the result.
		cout << "For component " << i << ": " << fixed << setprecision(2) << ((sum + 0.0) / sizeOfComponent) << endl;

		delete[] nodesInComponent;
	}

	delete[] connComponents;
}

void FriendNet::displayDiameters()
{
	//Get the connected components using the getConnectedComponents() function
	int lengthConnComponents;
	SimpleLinkedList* connComponents = getConnectedComponents(lengthConnComponents);

	//Print the first sentence of the information.
	cout << "There are " << lengthConnComponents << " connected components. The diameters are:" << endl;

	//For each connected component:
	for (int i = 0; i < lengthConnComponents; i++)
	{
		//Get the nodes in that connected components as an array.
		int sizeOfComponent;
		int* nodesInComponent = connComponents[i].getListAsArray(sizeOfComponent);

		int largestSoFar = 0; //Largest distance found so far between two nodes.

		int singleSourceLongest;
		for (int j = 0; j < sizeOfComponent; j++)
		{
			//Find the longest distance than can be obtained from some starting node.
			singleSourceLongest = singleSourceLongestDistance(nodesInComponent[j]);
			//If it is larger than the largest so far, update the largest so far.
			if (singleSourceLongest > largestSoFar)
				largestSoFar = singleSourceLongest;
		}
		delete[] nodesInComponent;

		//Print the information.
		cout << "For component " << i << ": " << largestSoFar << endl;
	}
	delete[] connComponents;
}

//Finds and returns the connected components as a linked list array.
SimpleLinkedList* FriendNet::getConnectedComponents(int& length)
{
	//First create an array of length numOfV since there can be at most numOfV connected components.
	//If there are less, we will handle it at the end by transporting them into a smaller array.
	SimpleLinkedList* connComponents = new SimpleLinkedList[numOfV];

	//the nodes that are seen so far.
	bool* seen = new bool[numOfV];
	for (int i = 0; i < numOfV; i++)
		seen[i] = false;

	int a = 0; //The number of the currently proccessed component.
	Stack stack; //Stack to do depth-first traversal.
	//Iterate over all the nodes.
	for (int i = 0; i < numOfV; i++)
	{
		//If this node is not yet put into a connected component,
		//use it as a starting point of a depth-first traversal to obtain a connected component.
		if (!seen[i])
		{
			stack.push(i);
			seen[i] = true;

			//This is now an element of ath connected component.
			connComponents[a].add(i);

			//Just do simple depth-first traversal,
			//add the encountered nodes into the connected component.
			while (!stack.isEmpty())
			{
				int popped = stack.pop();
				
				int adjLength;
				int* adjacents = adjList[popped].getListAsArray(adjLength);
				for (int i = 0; i < adjLength; i++)
				{
					if (!seen[adjacents[i]])
					{
						seen[adjacents[i]] = true;
						connComponents[a].add(adjacents[i]);
						stack.push(adjacents[i]);
					}
				}

				delete[] adjacents;
			}
			//After the depth-first traversal, we have encountered all the nodes
			//in that component. So increment a to start building the next connected component.
			a++;
		}
	}

	//Since we made the length of connComponents more than enough,
	//create a new array with the correct size now.
	SimpleLinkedList* connComponentsFinal = new SimpleLinkedList[a];
	for (int i = 0; i < a; i++)
	{
		connComponentsFinal[i] = connComponents[i];
	}
	delete[] seen;
	delete[] connComponents;
	length = a;
	return connComponentsFinal;
}

//Finds the largest distance from a given starting node to any other connected node. 
int FriendNet::singleSourceLongestDistance(int startingNode)
{
	Queue q1;
	Queue q2;

	//nodes that are seen so far
	bool* seen = new bool[numOfV];
	for (int i = 0; i < numOfV; i++)
		seen[i] = false;

	q1.enqueue(startingNode);
	seen[startingNode] = true;

	int dequeued;
	int* adjacents;
	int adjacentsSize;

	int longestDistance = 0;
	//Here, we do something similar to breadth-first traversal
	//to find the longest distance.
	while (!q1.isEmpty())
	{
		//dequeue all items from the first queue 
		//and enqueue them into the second queue
		while (!q1.isEmpty())
		{
			dequeued = q1.dequeue();
			q2.enqueue(dequeued);
		}
		//For all items from the second queue,
		//dequeue them and add their adjacent nodes into the first queue.
		while (!q2.isEmpty())
		{
			dequeued = q2.dequeue();
			adjacents = adjList[dequeued].getListAsArray(adjacentsSize);
			for (int i = 0; i < adjacentsSize; i++)
			{
				//Only add the non-seen nodes back.
				if (!seen[adjacents[i]])
				{
					q1.enqueue(adjacents[i]);
					seen[adjacents[i]] = true;
				}
			}

			delete[] adjacents;
		}

		//Every time we do the above operations without empyting the first queue,
		//we obtain another layer of nodes that can be obtained from the starting node.
		//Therefore increment the longest distance.
		if (!q1.isEmpty())
			longestDistance++;
	}

	delete[] seen;

	return longestDistance;
}
