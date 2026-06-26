#include "DeclareCommand.hpp"
#include "../Process/Process.hpp"
#include "../Process/ProcessResolver.hpp"

DeclareCommand::DeclareCommand(int p_id, std::string processName, std::string varName, uint16_t value, ProcessResolver& resolver)
    : ICommand(p_id, DECLARE), resolver(resolver) {
    this->processName = processName;
    this->varName = varName;
    this->value = value;
}

// Creates a text file to save the process logs
// 
void DeclareCommand::execute(int coreId) {
    // 1. Access process
    Process* process = resolver.getProcessOnCore(coreId);

    // 2. Access symbolTable in process
    process->symbolTable_setVar(varName, value);
}