#include "ForCommand.hpp"
#include "../process/Process.hpp"
#include "../process/ProcessResolver.hpp"
#include <string>
#include <vector>

ForCommand::ForCommand(int p_id, std::string processName, std::vector<std::shared_ptr<ICommand>> commandList, int repeats, ProcessResolver& resolver)
    : ICommand(p_id, ADD), resolver(resolver) {
    this->processName = processName;
    this->commandList = commandList;
    this->repeats = repeats;
}

// Creates a text file to save the process logs
// 
void ForCommand::execute(int coreId) {
    for (int i = 0; i < repeats; i++) {
        for (const auto& command: commandList) {
            command->execute(coreId);
        }
    }
}