#pragma once
#include "ICommand.hpp"
#include <string>
#include <optional>

class PrintCommand : public ICommand {
public:
    PrintCommand(int pid, const std::string& processName, const std::string& toPrint,
                    const std::optional<std::string>& varName = std::nullopt)
        : pid(pid), processName(processName), toPrint(toPrint), varName(varName) { };

    void execute(int coreId, Process& process) override;
    CommandType getCommandType() const override { return PRINT; }

private:
    int pid;
    std::string processName;
    std::string toPrint;
    std::optional<std::string> varName;

    static std::string getTimestamp();
};
