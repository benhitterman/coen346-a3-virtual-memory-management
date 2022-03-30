#include "include/memorymanager.h"

MemoryManager::MemoryManager(size_t maxPages, size_t k, unsigned int timeout)
    : maxPages(maxPages)
    , k(k)
    , timeout(timeout)
    , readyForRequest(false)
    , responseAvailable(false)
{
}

void MemoryManager::store(std::string id, unsigned int value)
{
    submitRequest(Operation::Store, id, value);
    requestSignal.notify_all();
}

void MemoryManager::release(std::string id)
{
    submitRequest(Operation::Release, id);
    requestSignal.notify_all();
}

unsigned int MemoryManager::lookup(std::string id)
{
    submitRequest(Operation::Lookup, id);

    // Create our response lock before notifying of a request to ensure that we begin waiting before a response can be generated
    std::unique_lock lock(responseMutex);
    requestSignal.notify_all();

    responseSignal.wait(lock, [this](){ return responseAvailable == true; });
    return responseValue;
}

void MemoryManager::submitRequest(Operation op, std::string id, unsigned int value = 0)
{
    // Lock the mutex allowing us to set the request parameters
    std::unique_lock lock(requestMutex);
    // If we receive the lock but readyForRequest is false then the memory manager is occupied, so wait
    if (!readyForRequest)
    {
        requestSignal.wait(lock, [this](){ return readyForRequest == true; });
    }

    // Set parameters for the request then indicate that nobody else should set anything
    requestOperation = op;
    requestId = id;
    requestValue = value;
    readyForRequest = false;
}

void MemoryManager::start()
{
    while (true) // TODO: Stop flag
    {
        std::unique_lock lock(requestMutex);
        requestSignal.wait(lock, [this](){ return readyForRequest == false; });

        // TODO: Handle request

        readyForRequest = true;
        lock.unlock();
        requestSignal.notify_all();
    }
}