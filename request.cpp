#include "include/request.h"

Request::Request(Operation op, std::string id, unsigned int value = 0)
    : operation(op)
    , id(id)
    , value(value)
{
}

Request::Operation Request::getOperation() const
{
    return operation;
}

std::string Request::getId() const
{
    return id;
}

unsigned int Request::getValue() const
{
    return value;
}