#include "include/request.h"

Request::Request(Operation op, int processId, std::string pageId, unsigned int value)
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

int Request::getProcessId() const
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