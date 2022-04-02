#ifndef PROCESS_H
#define PROCESS_H
class Process
{
private:
    int name;
    int startTime;
    int burstTime;

public:
    Process(int name, int startTime, int burstTime);

    int getName() const;
    int getstartTime() const;
    int getBurstTime() const;
};

#endif