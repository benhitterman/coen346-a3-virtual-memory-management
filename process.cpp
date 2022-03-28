#include "include/process.h"

Process::Process(std::string name, int arrivalTime, int burstTime)
    : name(name), arrivalTime(arrivalTime), burstTime(burstTime)
{
}
const std::string &Process::getName() const
{
    return name;
}

int Process::getArrivalTime() const
{
    return arrivalTime;
}

int Process::getBurstTime() const
{
    return burstTime;
}