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

    while (!arrivalQueue.empty() && arrivalQueue.peek()->getstartTime() == 0)
    {
        Process *p = arrivalQueue.pop();
    }
    while (!stopFlag)
    {
        while (!active->empty())
        {

            Process *cpuProcess = arrivalQueue.pop(); // TODO to put pop_back (Checks first item at bottom of vector)
            outFile << "Clock: " << clock.getTime() << ", Process" << cpuProcess->getName() << ": Started" << std::endl;

            while (clock.getTime() < cpuProcess->getBurstTime())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(Clock::pollingInterval));
            }

            // Join and delete the thread created to run the process
            outFile << "Clock: " << clock.getTime() << ", Process" << cpuProcess->getName() << ": Finished" << std::endl;
            processThreads[cpuProcess->getName()]->join();
            delete processThreads[cpuProcess->getName()];
            processThreads.erase(cpuProcess->getName());

            // Delete the process itself
            delete cpuProcess;
        }
    }
}