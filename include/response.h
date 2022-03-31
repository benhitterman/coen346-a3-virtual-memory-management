#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>

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