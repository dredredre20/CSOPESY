#include "ReadCommand.hpp"
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

void ReadCommand::execute(int coreID, Process& process) {
    IMemoryAllocator* allocator = process.getMemoryAllocator();
    void* handle = process.getMemoryAllocatedBlock();

    if (allocator == nullptr || handle == nullptr) {
        process.setMemoryViolation(currentTimestamp(), address);
        return;
    }

    try {
        uint16_t value = allocator->readMemory(handle, address);
        process.setVariable(varName, value);

        std::ostringstream oss;
        oss << "  \"READ " << varName << " <- [0x" << std::hex << address
            << std::dec << "] = " << value << "\"";
        process.addLog(oss.str());
    }
    catch (const MemoryAccessViolationException& e) {
        process.setMemoryViolation(currentTimestamp(), e.address);
    }
}