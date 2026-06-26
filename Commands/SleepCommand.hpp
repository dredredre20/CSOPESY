#pragma once
#include "ICommand.hpp"
#include "../Process/ProcessResolver.hpp"
#include <string>

class SleepCommand : public ICommand {
public:
    SleepCommand(int p_id, std::string processName, uint8_t X, ProcessResolver& resolver);
    void execute(int coreId) override;

private:
    std::string processName;
    ProcessResolver& resolver;
};
