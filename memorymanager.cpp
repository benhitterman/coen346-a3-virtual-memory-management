#include "include/memorymanager.h"

#include <thread>
#include <fstream>

#include "include/clock.h"

constexpr auto swapFile = "vm.txt";

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
                auto retVal = responseList[i].getValue();
                responseList.erase(responseList.begin() + i);
                return retVal;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Clock::pollingInterval));
    }
}

void MemoryManager::start()
{
    auto& clock = Clock::getInstance();
    while (true) // TODO: Stop flag
    {
        std::unique_lock requestLock(requestMutex);
        if (!requestQueue.empty())
        {
            Request r = requestQueue.front();
            requestQueue.pop();
            requestLock.unlock();

            switch (r.getOperation())
            {
                case Request::Operation::Store:
                    handleStore(r);
                    break;
                case Request::Operation::Release:
                    handleRelease(r);
                    break;
                case Request::Operation::Lookup:
                default:
                    break;
            }
        }
    }
}

void MemoryManager::handleStore(Request& r)
{
    Page newPage(r.getId(), r.getValue());
    if (mainMemory.size() < maxPages)
    {
        mainMemory.push_back(newPage);
    }
    else
    {
        std::fstream pageFile(swapFile, std::ios_base::app);
        pageFile << newPage << std::endl;
    }
}

void MemoryManager::handleRelease(Request& r)
{
    for (size_t i = 0; i < mainMemory.size(); i++)
    {
        if (mainMemory[i].getId() == r.getId())
        {
            mainMemory.erase(mainMemory.begin() + i);
            return;
        }
    }

    std::fstream pageFile(swapFile);

    std::vector<std::string> tempBuffer;
    std::string line;
    while (std::getline(pageFile, line))
    {
        size_t space = line.find(' ');
        if (line.substr(0, space) != r.getId())
        {
            tempBuffer.push_back(line);
        }
    }

    pageFile.seekp(0);
    for (auto& i : tempBuffer)
    {
        pageFile << i << std::endl;
    }
}