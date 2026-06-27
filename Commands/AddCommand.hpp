#pragma once
#include "ICommand.hpp"
#include <string>
#include <variant>
#include <cstdint>

class AddCommand : public ICommand {
public:
    using Operand = std::variant<std::string, uint16_t>;

    AddCommand(const std::string& result, Operand a, Operand b)
        : result(result), a(a), b(b) {
    }

    void execute(int coreID, Process& process) override;
    CommandType getCommandType() const override { return ADD; }

private:
    std::string result;
    Operand a, b;

    uint32_t resolve(const Operand& operand, Process& process) const;
};