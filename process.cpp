#include "include/process.h"
#include "include/nextCommand.h"
#include "include/clock.h"
#include "include/memorymanager.h"
#include <thread>

Process::Process(int id, int arrivalTime, int burstTime, nextCommand *nc1, MemoryManager *mm1)
    : id(id), arrivalTime(arrivalTime), burstTime(burstTime), nc1(nc1), mm1(mm1), isTerminated(false)
{
}

int Process::getArrivalTime() const
{
    return arrivalTime;
}

int Process::getBurstTime() const
{
    return burstTime;
}

bool Process::getIsTerminated()
{
    return isTerminated;
}

int Process::getId()
{
    return id;
}

void Process::runNextCommand()
{
    std::string command;
    Clock &clock = Clock::getInstance();
    int processEndTime = clock.getTime() + burstTime;
    int sleepTime, endTime;
    while (clock.getTime() < processEndTime)
    {
        command = nc1->getNextCommand();
        // parsing command
        size_t firstWhitespace = command.find(" ");
        std::string argument = command.substr(0, firstWhitespace);
        std::string value1 = command.substr(firstWhitespace + 1, 1);
        if (argument == "Store")
        {
            unsigned int value2 = stoi(command.substr(firstWhitespace + 3, 1));
            mm1->store(value1, value2);
        }
        else if (argument == "Lookup")
            mm1->lookup(value1);
        else if (argument == "Release")
            mm1->release(value1);

        sleepTime = rand() % 1000 + 100; // random value between 100 and 1000
        endTime = clock.getTime() + sleepTime;
        while (clock.getTime() < endTime && clock.getTime() < processEndTime)
            std::this_thread::sleep_for(std::chrono::milliseconds(Clock::pollingInterval));
    }
    isTerminated = true;
}
