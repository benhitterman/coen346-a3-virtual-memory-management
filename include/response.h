#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>

// Represents a response from the memory manager to a lookup request.
class Response
{
public:
    Response(std::string id, unsigned int value);

    std::string getId() const;
    unsigned int getValue() const;
    
private:
    std::string id;
    unsigned int value;
};

#endif