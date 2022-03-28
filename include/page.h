#ifndef PAGE_H
#define PAGE_H

#include <utility>
#include <string>

class Page
{
public:
    Page(std::string id, unsigned int value);

    std::string getId() const;
    unsigned int getValue() const;

private:
    std::pair<std::string, unsigned int> pair;
};

#endif