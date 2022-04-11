#include "include/response.h"

Response::Response(std::string processId, std::string pageId, unsigned int value)
    : processId(processId), pageId(pageId), value(value)
{
}

std::string Response::getProcessId() const
{
    return processId;
}

std::string Response::getPageId() const
{
    return pageId;
}

unsigned int Response::getValue() const
{
    return value;
}