#include "include/memorymanager.h"

#include <thread>

#include "include/clock.h"

MemoryManager::MemoryManager(size_t maxPages, size_t k, unsigned int timeout)
    : maxPages(maxPages)
    , k(k)
    , timeout(timeout)
{
}

void MemoryManager::store(std::string id, unsigned int value)
{
    std::lock_guard lock(requestMutex);
    requestQueue.push(Request(Request::Operation::Store, id, value));
}

void MemoryManager::release(std::string id)
{
    std::lock_guard lock(requestMutex);
    requestQueue.push(Request(Request::Operation::Release, id));
}

unsigned int MemoryManager::lookup(std::string id)
{
    std::unique_lock requestLock(requestMutex);

    requestQueue.push(Request(Request::Operation::Lookup, id));
    requestLock.unlock();

    while (true)
    {
        std::lock_guard responseLock(responseMutex);
        for (size_t i = 0; i < responseList.size(); i++)
        {
            if (responseList[i].getId() == id)
            {
                return responseList[i].getValue();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Clock::pollingInterval));      
    }
}

void MemoryManager::start()
{
    constexpr auto swapFile = "vm.txt";

    auto& clock = Clock::getInstance();
    while (true) // TODO: Stop flag
    {
        
    }
}