#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <condition_variable>
#include <mutex>

#include "page.h"

class MemoryManager
{
public:
    MemoryManager(size_t numPages, size_t k, unsigned int timeout);
    ~MemoryManager();

    void store(std::string id, unsigned int value);
    void release(std::string id);
    unsigned int lookup(std::string id);

    // Runs the memory manager. Should be started in a thread.
    void start();

private:
    enum Operation
    {
        Store,
        Release,
        Lookup
    };

    Operation nextOperation;
    std::string nextId;
    unsigned int nextValue;
    bool readyForData;
    std::condition_variable signal;
    std::mutex extMutex;

    std::vector<Page> mainMemory;
    std::map<std::string, unsigned int> last;
    std::map<std::string, std::vector<unsigned int>> hist;
};

#endif