#pragma once
#include <unordered_map>
#include <string>

class SymbolTable {
    public:
        void setVar(const std::string& varName, uint16_t value);
        uint16_t getVar(const std::string& varName) const;
        bool hasVar(const std::string& varName) const;

    private:
        std::unordered_map<std::string, uint16_t> table;
};