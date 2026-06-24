#include "SymbolTable.hpp"
#include <string>

void SymbolTable::setVar(std::string varName, uint16_t value) {
    this->table[varName] = value;
}

uint16_t SymbolTable::getVar(const std::string varName) {
    return table.at(varName);
}