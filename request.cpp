#include "include/request.h"

Request::Request(Operation op, std::string processId, std::string pageId, unsigned int value)
    : operation(op)
    , processId(processId)
    , pageId(pageId)
    , value(value)
{
}

Request::Operation Request::getOperation() const
{
    return operation;
}

std::string Request::getProcessId() const
{
    return processId;
}

std::string Request::getPageId() const
{
    return pageId;
}

unsigned int Request::getValue() const
{
    return value;
}