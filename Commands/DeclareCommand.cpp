#include "DeclareCommand.hpp"
#include "../Process/Process.hpp"

void DeclareCommand::execute(int coreID, Process& process) {
    // Only declare if not already declared
    if (!process.hasVariable(varName)) {
        process.setVariable(varName, initialValue);
    }
}