#pragma once
#include <unordered_map>
#include <string>

class SymbolTable {
public:
    static constexpr int MAX_SIZE_BYTES = 64;
    static constexpr int BYTES_PER_VAR = 2;
	static constexpr int MAX_VARS = MAX_SIZE_BYTES / BYTES_PER_VAR;

        void declareVar(const std::string& varName);
        uintptr_t getAddress(const std::string& varName) const;
        bool hasVar(const std::string& varName) const;

        int getUsedBytes() const { return static_cast<int>(table.size()) * BYTES_PER_VAR; }
		int getFreeBytes() const { return static_cast<int>(MAX_SIZE_BYTES) - getUsedBytes(); }
		int getUsedVars() const { return static_cast<int>(table.size()); }

    private:
		std::unordered_map<std::string, uintptr_t> table; // now stores the offset value of the variable in memory
};