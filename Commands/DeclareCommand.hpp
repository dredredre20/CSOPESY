#pragma once
#include "ICommand.hpp"
#include "../process/ProcessResolver.hpp"
#include <string>

class DeclareCommand : public ICommand {
public:
    DeclareCommand(int p_id, std::string processName, std::string varName, uint16_t value, ProcessResolver& resolver);
    void execute(int coreId) override;

private:
    std::string processName;
    std::string varName;
    uint16_t value;
    ProcessResolver& resolver;
};
