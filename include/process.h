#ifndef PROCESS_H
#define PROCESS_H
#include "include/nextCommand.h"
#include "include/memorymanager.h"

class Process
{
public:
    Process(int id, int arrivalTime, int burstTime, nextCommand *, MemoryManager *);
    Process(const Process &other);

    int getArrivalTime() const;
    int getBurstTime() const;
    void runNextCommand();
    bool getIsTerminated();
    int getId();

private:
    const int arrivalTime;
    const int burstTime;
    int id;
    nextCommand *nc1;
    MemoryManager *mm1;
    bool isTerminated;
};

#endif