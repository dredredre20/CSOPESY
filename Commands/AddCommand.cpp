#include "AddCommand.hpp"
#include "../Process/Process.hpp"
#include <algorithm>
#include <limits>
#include <sstream>

void AddCommand::execute(int coreID, Process& process) {
    uint32_t valA = resolve(a, process);
    uint32_t valB = resolve(b, process);

    // Clamp result to uint16 range
    uint32_t raw = valA + valB;
    uint16_t res = static_cast<uint16_t>(
        std::min(raw, static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()))
        );

    // Auto-declare result variable if needed
    if (!process.hasVariable(result))
        process.setVariable(result, 0);
    process.setVariable(result, res);

    std::ostringstream oss;
    oss << " \"ADD " << result << " = " << valA << " + " << valB << " -> " << res << "\"";
    process.addLog(oss.str());
}

uint32_t AddCommand::resolve(const Operand& operand, Process& process) const {
    if (std::holds_alternative<uint16_t>(operand))
        return std::get<uint16_t>(operand);

    const std::string& varName = std::get<std::string>(operand);
    // Auto-declare with 0 if not yet declared
    if (!process.hasVariable(varName))
        process.setVariable(varName, 0);
    return static_cast<uint32_t>(process.getVariable(varName));
}