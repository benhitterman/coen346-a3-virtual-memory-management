#ifndef NEXT_COMMAND_H
#define NEXT_COMMAND_H

#include <string>
#include <vector>
#include <mutex>

class nextCommand {
private:
    std::string command;
    std::vector<std::string> listOfCommands;
    int index;
    std::mutex sharedVariable;
public:
    nextCommand();
    nextCommand(std::string command);

    const std::string getCommand() const;
    std::vector<std::string> getListOfCommands();
    void findingListOfCommand(std::string);
    std::string getNextCommand();
};

#endif