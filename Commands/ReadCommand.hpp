#pragma once
#include "ICommand.hpp"
#include <string>
#include <cstdint>

class ReadCommand : public ICommand {
public:

    // format - (READ my_var 0x1000)
    ReadCommand(const std::string& varName, uintptr_t address)
        : varName(varName), address(address) {
    }

    void execute(int coreID, Process& process) override;
    CommandType getCommandType() const override { return READ; }

private:
    std::string varName;
    uintptr_t address;
};