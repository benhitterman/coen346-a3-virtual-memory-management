#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>

// Represents a response from the memory manager to a lookup request.
class Response
{
public:
    Response(std::string processId, std::string pageId, unsigned int value);

    std::string getProcessId() const;
    std::string getPageId() const;
    unsigned int getValue() const;
    
private:
    std::string processId;
    std::string pageId;
    unsigned int value;
};

#endif