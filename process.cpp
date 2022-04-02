#include "include/process.h"

Process::Process(int name, int startTime, int burstTime)
    : name(name), startTime(startTime), burstTime(burstTime)
{
}
int Process::getName() const
{
    return name;
}

int Process::getstartTime() const
{
    return startTime;
}

int Process::getBurstTime() const
{
    return burstTime;
}