#include <iostream>
#include <iomanip>
#include <fstream>
#include "maxHeap1.h"

/**
* Author : Rüzgar Ayan
* ID: 21801984
* Section : 2
* Assignment : 3
*/

//struct to keep the data of patients after parsing them.
struct patient
{
	int id;
	int priority;
	int arrivalTime;
	int serviceTime;
};

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout << "Incorrect command line arguments.";
		return 0;
	}

	maxHeap heap;
	ifstream in(argv[1]);
	int avgWaitingTime = stoi(argv[2]);

	int	patientNumber;
	in >> patientNumber;

	//Parse and store all data in the patients array.
	patient* patients = new patient[patientNumber];
	for (int i = 0; i < patientNumber; i++)
	{
		in >> patients[i].id >> patients[i].priority 
			>> patients[i].arrivalTime >> patients[i].serviceTime;
	}

	HeapPatient patientToAdd;
	bool found = false; //True if minimum number of doctors is found.
	bool simulated = false; //True if the simulation the correct number of doctors is complete.
	int numDoctor = 1; //Number of doctors to currently try.
	double sum;

	//Run this loop until the simulation is done.
	while (!simulated)
	{
		//If the minimum number of doctors is found, print these to start the simulation.
		if (found)
		{
			cout << "Minimum number of doctors required : " << numDoctor << endl << endl;
			cout << "Simulation with " << numDoctor << " doctors:" << endl << endl;
		}

		int currentPatient = 0; //Index of the next patient.
		int minutes = 1; //Current minute.
		bool doctorsNotWorking = true; //This is true if there are no doctors currently working.
		int* doctors = new int[numDoctor]; //This array shows the remaining service time of their patient for each doctor.
		sum = 0;
		//Reset the doctors array before starting the loop again.
		for (int i = 0; i < numDoctor; i++)
			doctors[i] = 0;

		//Run this loop while there are still patients to proccess or there are doctors still working.
		while (currentPatient < patientNumber || !doctorsNotWorking)
		{
			//Add new patients to the heap whose arrival times has come.
			//This works because the arrival times of the patients are given sorted.
			while (currentPatient < patientNumber 
				&& patients[currentPatient].arrivalTime == minutes)
			{
				patientToAdd.key1 = patients[currentPatient].priority;
				patientToAdd.key2 = patients[currentPatient].id;
				patientToAdd.item = currentPatient;
				heap.heapInsert(patientToAdd);
				currentPatient++;
			}

			doctorsNotWorking = true;
			for (int i = 0; i < numDoctor; i++)
			{
				//If doctor currently has a patient, decrease the remaining minutes of service time.
				if (doctors[i] != 0)
				{
					doctors[i]--;
					doctorsNotWorking = false;
				}
				//If a doctor is currently not working assign him the next patient from the heap.
				else
				{
					if (!heap.heapIsEmpty())
					{
						HeapPatient nextPatient;
						heap.heapDelete(nextPatient);
						int index = nextPatient.item;
						//If found is true, then this is the simulation part, so print the information.
						if (found)
						{
							cout << "Doctor " << i << " takes patient " << patients[index].id
								<< " at minute " << minutes << " (wait: "
								<< (minutes - patients[index].arrivalTime) << " min)" << endl;
						}
						sum += minutes - patients[index].arrivalTime;
						doctors[i] = patients[index].serviceTime - 1;
						doctorsNotWorking = false;
					}
				}
			}
			minutes++;
		}

		//Deallocate dynamic array doctors.
		delete[] doctors;

		//If this iteration was for the simulation, print the information and set simulated to true to quit the loop.
		if (found)
		{
			cout << endl << "Average waiting time : " << fixed << setprecision(2) << (sum / patientNumber) << " minutes" << endl;
			simulated = true;
		}
		//If the constraint is satisfied, set found to be true so that the next iteration will be the simulation.
		else if ((sum / patientNumber) < avgWaitingTime)
		{
			found = true;
		}
		//If the constraint is not satisfied, increase the numbor of doctors by one and try again.
		else
		{
			numDoctor++;
		}
	}

	delete[] patients;
}