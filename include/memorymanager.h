#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

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
    MemoryManager(size_t maxPages, size_t k, unsigned int timeout);

    void store(std::string id, unsigned int value);
    void release(std::string id);
    unsigned int lookup(std::string id);

    // Runs the memory manager. Should be started in a thread.
    void start();

private:
    void handleStore(Request& r);
    void handleRelease(Request& r);
    Response handleLookup(Request& r);

    const size_t maxPages;
    const size_t k;
    const unsigned int timeout;

    std::mutex requestMutex;
    std::queue<Request> requestQueue;

    std::mutex responseMutex;
    std::vector<Response> responseList;

    std::vector<Page> mainMemory;
    std::map<std::string, unsigned int> last;
    std::map<std::string, std::vector<unsigned int>> hist;
};

#endif