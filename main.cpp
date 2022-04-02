#include <iostream>
#include <fstream>
#include <vector>
#include "include/process.h"
#include "include/clock.h"
#include "include/scheduler.h"
int main()
{
    std::ifstream myfile;
    myfile.open("processes.txt");
    if (!myfile)
    {
        std::cout << "Can not open file.";
        exit(1);
    }
    int numOfCores, numOfProcess, arrivalTime, burstTime;
    std::string name;

    myfile >> numOfCores >> numOfProcess; // read number of cores and processes
    std::vector<Process *> processList;   // vector of Processes

    processList.reserve(numOfProcess); // Reserves number of processes in vector
    // Reads from file and instantiates objects that are added to the Vector
    for (int i = 0; i < numOfProcess; i++)
    {
        myfile >> arrivalTime >> burstTime;
        processList.push_back(new Process(i + 1, arrivalTime, burstTime));
    }
    myfile.close();
    return 0;
}