#pragma once
#include "ICommand.hpp"
#include <string>
#include <cstdint>

class DeclareCommand : public ICommand {
public:
    DeclareCommand(const std::string& varName, uint16_t initialValue)
        : varName(varName), initialValue(initialValue) {
    }

    void execute(int coreID, Process& process) override;
    CommandType getCommandType() const override { return DECLARE; }

private:
    std::string varName;
    uint16_t initialValue;
};