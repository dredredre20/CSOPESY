#include "ICommand.hpp"
#include <string>
#include <cstdint>

class WriteCommand : public ICommand {
public:

	// constructor for writing a non-literal value (e.g., WRITE 0x500 varA)
	WriteCommand(uintptr_t address, const std::string& varName)
		: address(address), varName(varName), isLiteral(false), literalValue(0) {
	}

	// constructor for writing a literal value (e.g., WRITE 0x500 42)
	WriteCommand(uintptr_t address, uint16_t literalValue)
		: address(address), varName(""), isLiteral(true), literalValue(literalValue) {
	}

	void execute(int coreID, Process& process) override;
	CommandType getCommandType() const override { return WRITE; }

private:
	uintptr_t address;
	std::string varName;
	bool isLiteral;
	uint16_t literalValue;
};