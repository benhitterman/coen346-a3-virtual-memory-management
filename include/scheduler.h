#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <map>
#include <thread>
#include <atomic>

#include "process.h"
#include "processarrivalqueue.h"

class Scheduler
{
private:
    ProcessArrivalQueue arrivalQueue;
    std::map<std::string, std::thread *> processThreads;

public:
    Scheduler(ProcessArrivalQueue &queue);
    void operator()(std::atomic_bool &stopFlag);
};
#endif;