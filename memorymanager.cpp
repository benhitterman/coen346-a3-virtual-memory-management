#include "include/memorymanager.h"

#include <thread>
#include <fstream>
#include <syncstream>
#include <limits>

#include "include/clock.h"

constexpr auto swapFile = "vm.txt";

MemoryManager::MemoryManager(size_t maxPages, size_t k, int timeout, std::ofstream* outputFile)
    : maxPages(maxPages)
    , k(k)
    , timeout(timeout)
    , outputFile(outputFile)
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

void MemoryManager::start(std::atomic_bool& stopFlag)
{
    auto& clock = Clock::getInstance();
    while (!stopFlag)
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
                    std::osyncstream(*outputFile) << "Clock: " << clock.getTime() << ", Store: Variable " << r.getId() << ", Value: " << r.getValue() << std::endl;
                    break;
                case Request::Operation::Release:
                    handleRelease(r);
                    std::osyncstream(*outputFile) << "Clock: " << clock.getTime() << ", Release: Variable " << r.getId() << std::endl;
                    break;
                case Request::Operation::Lookup:
                {
                    Response resp = handleLookup(r);
                    std::osyncstream(*outputFile) << "Clock: " << clock.getTime() << ", Lookup: Variable " << r.getId() << ", Value: " << r.getValue() << std::endl;
                    std::lock_guard responseLock(responseMutex);
                    responseList.push_back(resp);
                    break;
                }
                default:
                    throw std::domain_error("Request has invalid operation type");
            }
        }
        else
        {
            requestLock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(Clock::pollingInterval));
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
        size_t victimIndex = findVictimIndex();
        Page victim = mainMemory[victimIndex];

        // Create a new page object from the page to be moved into main memory then erase the line from the temp buffer.
        std::string targetLine = tempBuffer[targetIndex];
        size_t space = targetLine.find(' ');
        Page newPage(targetLine.substr(0, space), stoi(targetLine.substr(space + 1)));
        tempBuffer.erase(tempBuffer.begin() + targetIndex);

        // Add the victim to the temp buffer and erase it from main memory.
        std::stringstream sstream;
        sstream << victim;
        tempBuffer.push_back(sstream.str());
        mainMemory.erase(mainMemory.begin() + victimIndex);

        // Add the new page to main memory.
        mainMemory.push_back(newPage);
        int now = Clock::getInstance().getTime();
        last[newPage.getId()] = now;
        std::vector<int> newPageHist(k, 0);
        newPageHist[0] = now;
        hist[newPage.getId()] = newPageHist;

        // Write the temp buffer back into the file on disk.
        pageFile.seekp(0);
        for (auto& i : tempBuffer)
        {
            pageFile << i << std::endl;
        }

        std::osyncstream(*outputFile) << "Clock: " << Clock::getInstance().getTime() << "Memory Manager, SWAP: Variable " << newPage.getId() << " with Variable " << victim.getId() << std::endl; 
        return Response(newPage.getId(), newPage.getValue());
    }

    // If the id wasn't found in main memory or in the page file, return -1.
    return Response(r.getId(), -1);
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
    // Determine if any pages satisfy [last - hist(1) > timeout]
    bool anyTimeout = false;
    size_t firstIndex;
    for (size_t i = 0; i < mainMemory.size(); i++)
    {
        auto id = mainMemory[i].getId();
        if (last[id] - hist[id][0] > timeout)
        {
            anyTimeout = true;
            firstIndex = i;
            break;
        }
    }

    if (anyTimeout)
    {
        // Of all pages satisfying [last - hist(1) > timeout], choose the one with the minimum value of hist(k)
        size_t victimIndex = firstIndex;
        for (size_t i = victimIndex + 1; i < mainMemory.size(); i++)
        {
            auto currentId = mainMemory[i].getId();
            if (last[currentId] - hist[currentId][0] > timeout)
            {
                // If there is a tie in the values of hist(k), choose based on hist(1)
                if (hist[currentId][k - 1] < hist[mainMemory[victimIndex].getId()][k - 1])
                {
                    victimIndex = i;
                }
                else if (hist[currentId][k - 1] == hist[mainMemory[victimIndex].getId()][k - 1])
                {
                    if (hist[currentId][0] < hist[mainMemory[victimIndex].getId()][0])
                    {
                        victimIndex = i;
                    }
                }
            }
        }
        return victimIndex;
    }
    else
    {
        // If no pages satisfy [last - hist(1) > timeout], choose the one with the minimum value of hist(1)
        size_t victimIndex = 0;
        for (size_t i = 1; i < mainMemory.size(); i++)
        {
            auto currentId = mainMemory[i].getId();
            if (hist[currentId][0] < hist[mainMemory[victimIndex].getId()][0])
            {
                victimIndex = i;
            }
        }
        return victimIndex;
    }
}