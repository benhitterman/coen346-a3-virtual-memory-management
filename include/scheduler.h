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
    std::map<int, std::thread *> processThreads;

public:
    Scheduler(ProcessArrivalQueue &queue);
    void operator()(std::atomic_bool &stopFlag);
};
#endif