#include "SymbolTable.hpp"
#include <string>
#include <stdexcept>

void SymbolTable::declareVar(const std::string& varName) {
    if (table.count(varName))
        return; // variable already declared

    if (table.size() >= MAX_VARS)
		throw std::runtime_error("Symbol table is full. Cannot declare more variables.");

    uintptr_t offset = static_cast<uintptr_t>(table.size() * BYTES_PER_VAR);
    table[varName] = offset;
}

uintptr_t SymbolTable::getAddress(const std::string& varName) const {
    auto it = table.find(varName);
    if (it == table.end())
        throw std::runtime_error("Variable not found.");
    return it->second;
}

bool SymbolTable::hasVar(const std::string& varName) const {
    return table.count(varName) > 0;
}