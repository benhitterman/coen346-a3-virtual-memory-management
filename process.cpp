#include "include/process.h"
#include "include/nextCommand.h"
#include "include/clock.h"
#include "include/memorymanager.h"
#include <thread>

Process::Process(int id, int arrivalTime, int burstTime, nextCommand *nc1, MemoryManager *mm1, std::ofstream* outputFile)
    : id(id), arrivalTime(arrivalTime), burstTime(burstTime), nc1(nc1), mm1(mm1), outputFile(outputFile), isTerminated(false)
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
            mm1->submitRequest(Request(Request::Operation::Store, id, value1, value2));
            std::osyncstream(*outputFile) << "Clock: " << clock.getTime() << ", Process " << id << ", Store: Variable " << value1 << ", Value: " << value2 << std::endl;
        }
        else if (argument == "Lookup")
        {
            mm1->submitRequest(Request(Request::Operation::Lookup, id, value1));
            Response r = mm1->getResponse(id);
            std::osyncstream(*outputFile) << "Clock: " << clock.getTime() << ", Process " << id << ", Lookup: Variable " << value1 << ", Value: " << r.getValue() << std::endl;
        }
        else if (argument == "Release")
        {
            mm1->submitRequest(Request(Request::Operation::Release, id, value1));
            std::osyncstream(*outputFile) << "Clock: " << clock.getTime() << ", Process " << id << ", Release: Variable " << value1 << std::endl;
        }

        sleepTime = rand() % 1000 + 100; // random value between 100 and 1000
        endTime = clock.getTime() + sleepTime;
        while (clock.getTime() < endTime && clock.getTime() < processEndTime)
            std::this_thread::sleep_for(std::chrono::milliseconds(Clock::pollingInterval));
    }
    isTerminated = true;
}
