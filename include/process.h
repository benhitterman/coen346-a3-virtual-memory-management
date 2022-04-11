#ifndef PROCESS_H
#define PROCESS_H

#include <fstream>
#include <syncstream>
#include "include/nextCommand.h"
#include "include/memorymanager.h"

class Process
{
public:
    Process(int id, int arrivalTime, int burstTime, nextCommand *, MemoryManager *, std::ofstream* outputFile);
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
    std::ofstream* outputFile;
    bool isTerminated;
};

#endif