#include "WriteCommand.hpp"
#include "../process/Process.hpp"
#include "../Memory/IMemoryAllocator.hpp" // adjust path to your actual folder
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

namespace {
    std::string currentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tmStruct;
#if defined(_WIN32)
        localtime_s(&tmStruct, &t);
#else
        localtime_r(&t, &tmStruct);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tmStruct, "%H:%M:%S");
        return oss.str();
    }
}

void WriteCommand::execute(int coreID, Process& process) {
	IMemoryAllocator* allocator = process.getMemoryAllocator();
	void* handle = process.getMemoryAllocatedBlock();

	if (allocator == nullptr || handle == nullptr) {
		process.setMemoryViolation(currentTimestamp(), address);
		return;
	}

	// check if the variable exists in the process's symbol table
	uint16_t valueToWrite = isLiteral ? literalValue :
		(process.hasVariable(varName) ? process.getVariable(varName) : 0);

	try {
		allocator->writeMemory(handle, address, valueToWrite);

		std::ostringstream oss;
		oss << "  \"WRITE [0x" << std::hex << address << std::dec
			<< "] <- " << valueToWrite;
		if (!isLiteral) oss << " (from " << varName << ")\"";

		process.addLog(oss.str());
	}
	catch (const MemoryAccessViolationException& e) {
		process.setMemoryViolation(currentTimestamp(), e.address);
	}
}