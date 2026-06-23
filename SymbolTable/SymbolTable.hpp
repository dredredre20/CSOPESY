#pragma once
#include <unordered_map>
#include <string>

class SymbolTable {
    public:
        SymbolTable();

        void setVar(std::string varName, uint16_t value) {
            this->table[varName] = value;
        }

        uint16_t getVar(const std::string varName) {
            return table[varName];
        };

    private:
        std::unordered_map<std::string, uint16_t> table;
};