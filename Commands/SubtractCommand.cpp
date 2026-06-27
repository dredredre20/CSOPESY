#include "SubtractCommand.hpp"
#include "../Process/Process.hpp"
#include <limits>

void SubtractCommand::execute(int coreID, Process& process) {
    uint32_t valA = resolve(a, process);
    uint32_t valB = resolve(b, process);

    // Clamp to 0, no negative
    uint16_t res = static_cast<uint16_t>(valA >= valB ? valA - valB : 0);

    if (!process.hasVariable(result))
        process.setVariable(result, 0);
    process.setVariable(result, res);
}

uint32_t SubtractCommand::resolve(const Operand& operand, Process& process) const {
    if (std::holds_alternative<uint16_t>(operand))
        return std::get<uint16_t>(operand);

    const std::string& varName = std::get<std::string>(operand);
    if (!process.hasVariable(varName))
        process.setVariable(varName, 0);
    return static_cast<uint32_t>(process.getVariable(varName));
}