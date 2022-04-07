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
    auto active = &arrivalQueue;

    while (!arrivalQueue.empty() && arrivalQueue.peek()->getArrivalTime() == 0)
    {
        Process *p = arrivalQueue.pop();
    }
    while (!stopFlag)
    {
        while (!active->empty())
        {

            Process *cpuProcess = arrivalQueue.pop();
            outFile << "Clock: " << clock.getTime() << ", Process" << cpuProcess->getId() << ": Started" << std::endl;
            std::thread(&Process::runNextCommand, *cpuProcess);

            while (clock.getTime() < cpuProcess->getBurstTime())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(Clock::pollingInterval));
            }

            // Join and delete the thread created to run the process
            outFile << "Clock: " << clock.getTime() << ", Process" << cpuProcess->getId() << ": Finished" << std::endl;
            processThreads[cpuProcess->getId()]->join();
            delete processThreads[cpuProcess->getId()];
            processThreads.erase(cpuProcess->getId());

            // Delete the process itself
            delete cpuProcess;
        }
    }
}