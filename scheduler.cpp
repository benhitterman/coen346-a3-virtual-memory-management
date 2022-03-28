#include "include/scheduler.h"

Scheduler::Scheduler(ProcessArrivalQueue &queue)
    : arrivalQueue(queue)
{
}

void Scheduler::operator()(std::atomic_bool &stopFlag)
{
}