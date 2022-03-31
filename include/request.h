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

    Request(Operation op, std::string id, unsigned int value = 0);

    Operation getOperation() const;
    std::string getId() const;
    unsigned int getValue() const;

private:
    Operation operation;
    std::string id;
    unsigned int value;
};

#endif