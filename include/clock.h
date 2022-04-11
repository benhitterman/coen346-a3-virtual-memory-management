#ifndef CLOCK_H
#define CLOCK_H

#include <shared_mutex>

// Simulates the clock used by the OS when scheduling processes. Thread-safe, uses the singleton pattern to share one instance through getInstance().
// Singleton pattern based on: https://stackoverflow.com/a/1008289
class Clock
{
public:
    /*
    * NB: These constants are chosen to govern the simulation speed.
    * maxCycles: The maximum number of times the clock should increment before terminating.
    * cycleLength: The number of REAL milliseconds to wait between each increment of the clock.
    * cycleIncrement: The number of SIMULATED milliseconds to increment the clock by each cycle.
    * pollingInterval: The number of REAL milliseconds dependent threads (scheduler, processes, etc) should wait between each time check.
    * The clock will end with a SIMULATED value of (maxCycles) * (cycleIncrement) and will take approximately (maxCycles) * (cycleLength) REAL milliseconds to terminate.
    */
    static constexpr unsigned int maxCycles = 160U;
    static constexpr unsigned int cycleLength = 250U;
    static constexpr unsigned int cycleIncrement = 50U;
    static constexpr unsigned int pollingInterval = 5U;

    static Clock& getInstance();

public:
    Clock(const Clock& other) = delete;
    void operator=(const Clock& other) = delete;

    int getTime() const;
    void start();

private:
    Clock();
    mutable std::shared_mutex mutex;
    int time;
    bool started;
};

#endif
