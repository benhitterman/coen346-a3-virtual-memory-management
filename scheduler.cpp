#include "include/scheduler.h"

Scheduler::Scheduler(ProcessArrivalQueue &queue)
    : arrivalQueue(queue)
{
}

void Scheduler::operator()(std::atomic_bool &stopFlag)
{
    Clock &clock = Clock::getInstance();
    std::ofstream outFile;
    outFile.open("output.txt");

    while (!arrivalQueue.empty() && arrivalQueue.peek()->getArrivalTime() == 0)
    {
        Process *p = arrivalQueue.peek();
        arrivalQueue.pop();
        readyQueue.push(p);
    }
    while (!stopFlag)
    {
        // Check for new processes to start
        if (!readyQueue.empty() && runningProcesses.size() < numCores)
        {
            Process *cpuProcess = readyQueue.front();
            readyQueue.pop();

            outFile << "Clock: " << clock.getTime() << ", Process" << cpuProcess->getId() << ": Started" << std::endl;
            runningProcesses.push_back(cpuProcess);
            processThreads[cpuProcess->getId()] = new std::thread(&Process::runNextCommand, *cpuProcess);
        }

        // Check if any running process are finished
        for (auto p : runningProcesses)
        {
            // TODO CHECK IF PROCESS IS TERMINATED


            // Join and delete the thread created to run the process
            outFile << "Clock: " << clock.getTime() << ", Process" << cpuProcess->getId() << ": Finished" << std::endl;
            processThreads[cpuProcess->getId()]->join();
            delete processThreads[cpuProcess->getId()];
            processThreads.erase(cpuProcess->getId());

            // Delete the process itself
            delete cpuProcess;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(Clock::pollingInterval));
    }
}