#include "DeclareCommand.hpp"
#include "../Process/Process.hpp"
#include <sstream>

void DeclareCommand::execute(int coreID, Process& process) {
    // Only declare if not already declared
    if (!process.hasVariable(varName)) {
        process.setVariable(varName, initialValue);
    }

    std::ostringstream oss;
    oss << "  \"DECLARE " << varName << " = " << initialValue << "\"";
    process.addLog(oss.str());
}