#include "include/response.h"

Response::Response(std::string id, unsigned int value)
    : id(id), value(value)
{
}

std::string Response::getId() const
{
    return id;
}

unsigned int Response::getValue() const
{
    return value;
}