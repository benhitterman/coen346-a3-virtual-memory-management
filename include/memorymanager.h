#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <atomic>
#include <fstream>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "page.h"
#include "request.h"
#include "response.h"

class MemoryManager
{
public:
    MemoryManager(size_t maxPages, size_t k, int timeout, std::ofstream* outputFile);

    void store(int processId, std::string pageId, unsigned int value);
    void release(int processId, std::string pageId);
    unsigned int lookup(int processId, std::string pageId);

    // Runs the memory manager. Should be started in a thread.
    void start(std::atomic_bool& stopFlag);

private:
    void handleStore(Request& r);
    void handleRelease(Request& r);
    Response handleLookup(Request& r);

    void updateTimestamps(Page& p);
    size_t findVictimIndex();
    std::vector<std::string> readVmFile();
    void writeVmFile(std::vector<std::string>& buffer);

    const size_t maxPages;
    const size_t k;
    const int timeout;

    std::ofstream* outputFile;

    std::mutex requestMutex;
    std::queue<Request> requestQueue;

    std::mutex responseMutex;
    std::vector<Response> responseList;

    std::vector<Page> mainMemory;
    std::map<std::string, int> last;
    std::map<std::string, std::vector<int>> hist;
};

#endif