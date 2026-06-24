#pragma once
#include <unordered_map>
#include <string>

class SymbolTable {
    public:
        void setVar(std::string varName, uint16_t value);
        uint16_t getVar(const std::string varName);

    private:
        std::unordered_map<std::string, uint16_t> table;
};