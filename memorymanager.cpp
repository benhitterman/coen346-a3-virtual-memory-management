#include "include/memorymanager.h"

#include <thread>
#include <fstream>
#include <limits>

#include "include/clock.h"

constexpr auto swapFile = "vm.txt";

MemoryManager::MemoryManager(size_t maxPages, size_t k, int timeout)
    : maxPages(maxPages)
    , k(k)
    , timeout(timeout)
{
}

void MemoryManager::store(std::string id, unsigned int value)
{
    // Push a new store request into the queue, then return.
    std::lock_guard lock(requestMutex);
    requestQueue.push(Request(Request::Operation::Store, id, value));
}

void MemoryManager::release(std::string id)
{
    // Push a new release request into the queue, then return.
    std::lock_guard lock(requestMutex);
    requestQueue.push(Request(Request::Operation::Release, id));
}

unsigned int MemoryManager::lookup(std::string id)
{
    // Push a new lookup request into the queue.
    std::unique_lock requestLock(requestMutex);
    requestQueue.push(Request(Request::Operation::Lookup, id));
    requestLock.unlock();

    // Busy-wait until our response has been added into the response list.
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
    // Place the new page in main memory, if there's space.
    if (mainMemory.size() < maxPages)
    {
        int now = Clock::getInstance().getTime();

        mainMemory.push_back(newPage);

        // When a page is added, last is initialized to the current time.
        last[newPage.getId()] = now;
        // Create a vector of k zeros to represent the lookup history and initialize the zeroth index to the current time.
        std::vector<int> newPageHist(k, 0);
        newPageHist[0] = now;
        hist[newPage.getId()] = newPageHist;
    }
    else // Otherwise add the new page to the swap file.
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

Response MemoryManager::handleLookup(Request& r)
{
    for (size_t i = 0; i < mainMemory.size(); i++)
    {
        if (mainMemory[i].getId() == r.getId())
        {
            updateTimestamps(mainMemory[i]);
            return Response(mainMemory[i].getId(), mainMemory[i].getValue());
        }
    }

    std::fstream pageFile(swapFile);
    std::vector<std::string> tempBuffer;
    // Default to an unrealistically large value
    size_t targetIndex = std::numeric_limits<size_t>::max();
    std::string line;
    while (std::getline(pageFile, line))
    {
        size_t space = line.find(' ');
        if (line.substr(0, space) == r.getId())
        {
            // If tempBuffer has size n, then indices 0..(n-1) are occupied and the next push will be index n.
            targetIndex = tempBuffer.size();
        }
        tempBuffer.push_back(line);
    }

    // We found the target in the page file, now we need to swap
    if (targetIndex != std::numeric_limits<size_t>::max())
    {
        
    }
}

void MemoryManager::updateTimestamps(Page& p)
{
    int now = Clock::getInstance().getTime();
    std::string id = p.getId();
    if (now - last[id] <= timeout)
    {
        last[id] = now;
    }
    else // now - last > timeout
    {
        int lcp = last[id] - hist[id][0];
        for (size_t i = k - 1; i > 0; i--)
        {
            hist[id][i] = hist[id][i - 1] + lcp;
        }
        hist[id][0] = now;
        last[id] = now;
    }
}

size_t MemoryManager::findVictimIndex()
{
    
}