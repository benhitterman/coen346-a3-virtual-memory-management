#include "include/nextcommand.h"
#include <fstream>
#include <iostream>

nextCommand::nextCommand(){}

nextCommand::nextCommand(std::string command)
    : command(command),
    index{0}
{}

const std::string nextCommand::getCommand() const {
    return command;
}
std::vector<std::string> nextCommand::getListOfCommands() {
    return listOfCommands;
}

void nextCommand::findingListOfCommand(std::string textFile) {
    std::string command;
    
    std::ifstream myFileRead(textFile);

    if(!myFileRead) {
        std::cout <<"Can not open file.";
    }
    else {
    while(getline(myFileRead, command)) {
        listOfCommands.push_back(command);
    }
    }
}

std::string nextCommand::getNextCommand() {
    std::string nextCommand;
    std::lock_guard lock(sharedVariable);
    
    if (index < listOfCommands.size())
        return listOfCommands[index++];
    else {
        index = 0;
        return listOfCommands[index++];
    }
}
