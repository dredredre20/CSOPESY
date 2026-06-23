#include "AddCommand.hpp"
#include "../process/Process.hpp"
#include "../process/ProcessResolver.hpp"
#include <charconv>   // std::from_chars
#include <cstdint>
#include <string>

AddCommand::AddCommand(int p_id, std::string processName, std::string varNameD, std::string value1, std::string value2, ProcessResolver& resolver)
    : ICommand(p_id, ADD), resolver(resolver) {
    this->processName = processName;
    this->varNameD = varNameD;
    this->value1 = value1;
    this->value2 = value2;
}

// Creates a text file to save the process logs
// 
void AddCommand::execute(int coreId) {
    // 1. Access process
    Process* process = resolver.getProcessOnCore(coreId);

    // 2. Check our inputs (to see if they are variable names/uint_16t)
    uint16_t operand1 = resolveOperand(process, value1);
    uint16_t operand2 = resolveOperand(process, value2);

    uint16_t buffer = operand1 + operand2; // consider overflow handling, see below
    process->symbolTable_setVar(varNameD, buffer);
}

uint16_t AddCommand::resolveOperand(Process* process, const std::string& token) {
    uint16_t parsed = 0;
    auto result = std::from_chars(token.data(), token.data() + token.size(), parsed);

    bool isWholeStringNumeric =
        result.ec == std::errc() && result.ptr == token.data() + token.size();

    if (isWholeStringNumeric) {
        return parsed; // it was a literal, e.g. "42"
    }

    // Not a number → treat it as a variable name and look it up.
    return process->symbolTable_getVar(token);
}