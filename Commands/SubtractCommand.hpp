#pragma once
#include "ICommand.hpp"
#include "../process/ProcessResolver.hpp"
#include <string>

class SubtractCommand : public ICommand {
public:
    SubtractCommand(int p_id, std::string processName, std::string varNameD, std::string value1, std::string value2, ProcessResolver& resolver);
    void execute(int coreId) override;
    uint16_t resolveOperand(Process* process, const std::string& token);

private:
    std::string processName;
    std::string varNameD;
    std::string value1;
    std::string value2;
    ProcessResolver& resolver;
};
