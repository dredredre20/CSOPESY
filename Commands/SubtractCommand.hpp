#pragma once
#include "ICommand.hpp"
#include <string>
#include <variant>
#include <cstdint>

class SubtractCommand : public ICommand {
public:
    using Operand = std::variant<std::string, uint16_t>;

    SubtractCommand(const std::string& result, Operand a, Operand b)
        : result(result), a(a), b(b) {
    }

    void execute(int coreID, Process& process) override;
    CommandType getCommandType() const override { return SUBTRACT; }

private:
    std::string result;
    Operand a, b;

    uint32_t resolve(const Operand& operand, Process& process) const;
};
