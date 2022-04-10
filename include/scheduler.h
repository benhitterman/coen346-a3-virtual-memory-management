#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <map>
#include <thread>
#include <atomic>
#include <fstream>
#include "process.h"
#include "processarrivalqueue.h"
#include "clock.h"

class Scheduler
{
private:
    ProcessArrivalQueue arrivalQueue;
    std::queue<Process *> readyQueue;
    std::vector<Process *> runningProcesses;
    std::map<int, std::thread *> processThreads;
    int numCores;

public:
    Scheduler(ProcessArrivalQueue &queue, int numCores);
    void start(std::atomic_bool &stopFlag);
};
#endif