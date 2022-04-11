#ifndef REQUEST_H
#define REQUEST_H

#include <string>

// Represents a pending request to the memory manager.
class Request
{
public:
    enum Operation
    {
        Store,
        Release,
        Lookup
    };

    Request(Operation op, int processId, std::string pageId, unsigned int value = 0);

    Operation getOperation() const;
    int getProcessId() const;
    std::string getPageId() const;
    unsigned int getValue() const;

private:
    Operation operation;
    int processId;
    std::string pageId;
    unsigned int value;
};

#endif