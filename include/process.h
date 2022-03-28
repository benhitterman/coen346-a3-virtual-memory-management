#ifndef PROCESS_H
#define PROCESS_H
#include <string>
class Process
{
private:
    std::string name;
    int arrivalTime;
    int burstTime;

public:
    Process(std::string name, int arrivalTime, int burstTime);

    const std::string &getName() const;
    int getArrivalTime() const;
    int getBurstTime() const;
};

#endif