#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include "include/clock.h"
#include "include/memorymanager.h"
#include "include/nextCommand.h"
#include "include/process.h"
#include "include/scheduler.h"

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
    MemoryManager *memory = new MemoryManager(maxPages, k, timeout);
    myfile.close();

    // Reading Commands text
    nextCommand *commands = new nextCommand();
    commands->findingListOfCommand("commands.txt");

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
        processList.push_back(new Process(i + 1, arrivalTime, burstTime, commands, memory));
    }
    myfile.close();
    Clock &clock = Clock::getInstance();
    ProcessArrivalQueue queue(processList);
    Scheduler scheduler(queue, numOfCores);

    std::atomic_bool stopFlag;
    std::thread clockThread(&Clock::start, &clock);
    std::thread memoryManagerThread(&MemoryManager::start, memory, std::ref(stopFlag));
    std::thread schedulerThread(&Scheduler::start, &scheduler, std::ref(stopFlag));

    clockThread.join();
    stopFlag = true;
    memoryManagerThread.join();
    schedulerThread.join();

    return 0;
}