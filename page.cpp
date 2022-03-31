#include "include/page.h"

Page::Page(std::string id, unsigned int value)
    : pair(std::make_pair(id, value))
{
}

std::string Page::getId() const
{
    return std::get<0>(pair);
}

unsigned int Page::getValue() const
{
    return std::get<1>(pair);
}

std::ostream& operator<<(std::ostream& out, const Page& p)
{
    out << p.getId() << ' ' << p.getValue();
    return out;
}