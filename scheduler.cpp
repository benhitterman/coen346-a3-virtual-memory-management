#include "include/scheduler.h"

Scheduler::Scheduler(ProcessArrivalQueue &queue, int numCores)
    : arrivalQueue(queue), numCores(numCores)
{
}

void Scheduler::start(std::atomic_bool &stopFlag)
{
    Clock &clock = Clock::getInstance();
    std::ofstream outFile;
    outFile.open("output.txt");

    while (!arrivalQueue.empty() && arrivalQueue.peek()->getArrivalTime() == 0)
    {
        Process *p = arrivalQueue.peek();
        arrivalQueue.pop();
        readyQueue.push(p);
        outFile << "Clock: " << clock.getTime() << ", Process " << p->getId() << ": Arrived" << std::endl;
    }
    while (!stopFlag)
    {
        // Check if new processes have arrived
        while (!arrivalQueue.empty() && arrivalQueue.peek()->getArrivalTime() <= clock.getTime())
        {
            Process *p = arrivalQueue.peek();
            arrivalQueue.pop();
            readyQueue.push(p);
            outFile << "Clock: " << clock.getTime() << ", Process " << p->getId() << ": Arrived" << std::endl;
        }

        // Check for new processes to start
        while (!readyQueue.empty() && runningProcesses.size() < numCores)
        {
            Process *cpuProcess = readyQueue.front();
            readyQueue.pop();

            outFile << "Clock: " << clock.getTime() << ", Process " << cpuProcess->getId() << ": Started" << std::endl;
            runningProcesses.push_back(cpuProcess);
            processThreads[cpuProcess->getId()] = new std::thread(&Process::runNextCommand, cpuProcess);
        }

        // Check if any running process are finished
        std::vector<size_t> terminatedIndices;
        for (size_t i = 0; i < runningProcesses.size(); i++)
        {
            if (runningProcesses[i]->getIsTerminated())
            {
                terminatedIndices.push_back(i);
            }
        }
        for (auto i : terminatedIndices)
        {
            Process* p = runningProcesses[i];
            outFile << "Clock: " << clock.getTime() << ", Process " << p->getId() << ": Finished" << std::endl;

            runningProcesses.erase(runningProcesses.begin() + i);

            // Join and delete the thread created to run the process
            processThreads[p->getId()]->join();
            delete processThreads[p->getId()];
            processThreads.erase(p->getId());
            // Delete the process itself
            delete p;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(Clock::pollingInterval));
    }
}