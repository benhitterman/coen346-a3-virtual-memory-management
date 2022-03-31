#ifndef PAGE_H
#define PAGE_H

#include <utility>
#include <string>
#include <iostream>

class Page
{
public:
    Page(std::string id, unsigned int value);

    std::string getId() const;
    unsigned int getValue() const;

    friend std::ostream& operator<<(std::ostream& out, const Page& p);

private:
    std::pair<std::string, unsigned int> pair;
};

#endif