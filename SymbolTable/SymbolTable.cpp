#include "SymbolTable.hpp"
#include <string>

void SymbolTable::setVar(const std::string& varName, uint16_t value) {
    table[varName] = value;
}

uint16_t SymbolTable::getVar(const std::string& varName) const {
    return table.at(varName);
}

bool SymbolTable::hasVar(const std::string& varName) const {
    return table.count(varName) > 0;
}