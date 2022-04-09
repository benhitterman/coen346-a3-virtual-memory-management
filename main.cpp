#include <iostream>
#include <fstream>
#include <vector>
#include "include/process.h"
#include "include/clock.h"
#include "include/scheduler.h"
#include "include/nextCommand.h"
#include "include/memorymanager.h"
int main()
{
    std::ifstream myfile;
    // Reading memory input text
    myfile.open("memconfig.txt");
    if (!myfile)
    {
        std::cout << "Can not open file.";
        exit(1);
    }
    size_t maxPages, k;
    int timeout;
    myfile >> maxPages >> k >> timeout;
    MemoryManager memory(maxPages, k, timeout);
    myfile.close();

    // Reading Commands text
    myfile.open("commands.txt");
    if (!myfile)
    {
        std::cout << "Can not open file.";
        exit(1);
    }

    myfile.close();

    // Reading list of Processes
    myfile.open("processes.txt");
    if (!myfile)
    {
        std::cout << "Can not open file.";
        exit(1);
    }
    int numOfCores, numOfProcess, arrivalTime, burstTime;

    myfile >> numOfCores >> numOfProcess; // read number of cores and processes
    std::vector<Process *> processList;   // vector of Processes

    processList.reserve(numOfProcess); // Reserves number of processes in vector
    // Reads from file and instantiates objects that are added to the Vector
    for (int i = 0; i < numOfProcess; i++)
    {
        myfile >> arrivalTime >> burstTime;
        processList.push_back(new Process(i + 1, arrivalTime, burstTime, memory *));
    }
    myfile.close();
    Clock &clock = Clock::getInstance();
    ProcessArrivalQueue queue(processList);
    Scheduler scheduler(queue, numOfCores);

    return 0;
}