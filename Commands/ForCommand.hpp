#pragma once
#include "ICommand.hpp"
#include "../process/ProcessResolver.hpp"
#include <string>
#include <vector>

class ForCommand : public ICommand {
public:
    ForCommand(int p_id, std::string processName, std::vector<std::shared_ptr<ICommand>> commandList, int repeats, ProcessResolver& resolver);
    void execute(int coreId) override;

private:
    std::string processName;
    std::vector<std::shared_ptr<ICommand>> commandList;
    int repeats;
    ProcessResolver& resolver;
};
