#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <atomic>

#include "page.h"

class MemoryManager
{
public:
    MemoryManager(size_t maxPages, size_t k, unsigned int timeout);

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

    void submitRequest(Operation op, std::string id, unsigned int value = 0);
    
    const size_t maxPages;
    const size_t k;
    const unsigned int timeout;

    Operation requestOperation;
    std::string requestId;
    unsigned int requestValue;
    std::atomic_bool readyForRequest;
    std::condition_variable requestSignal;
    std::mutex requestMutex;

    unsigned int responseValue;
    std::atomic_bool responseAvailable;
    std::condition_variable responseSignal;
    std::mutex responseMutex;

    std::vector<Page> mainMemory;
    std::map<std::string, unsigned int> last;
    std::map<std::string, std::vector<unsigned int>> hist;
};

#endif