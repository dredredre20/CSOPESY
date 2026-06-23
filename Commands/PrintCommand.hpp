#pragma once
#include "ICommand.hpp"
#include <string>

class PrintCommand : public ICommand {
public:
    PrintCommand(int p_id, std::string processName, std::string toPrint);
    void execute(int coreId) override;

private:
    std::string processName;
    std::string toPrint;

    static std::string getTimestamp();
};
