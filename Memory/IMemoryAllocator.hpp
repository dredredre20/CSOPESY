#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <deque>
#include <memory>

class Process;

// Thrown when a process tries to access memory that it does not have permission to access.
class MemoryAccessViolationException : public std::runtime_error {
public:
    uintptr_t address;
	// use of explicit to prevent implicit conversions
    explicit MemoryAccessViolationException(uintptr_t addr)
        : std::runtime_error("Memory Access Violation"), address(addr) {}
};

class IMemoryAllocator {
    public:
        virtual ~IMemoryAllocator() = default;
        virtual void* allocate(std::shared_ptr<Process> process) = 0;
        virtual void deallocate(void* ptr) = 0;

        // virtual std::string visualizeMemory(std::string command) = 0;
        virtual std::string visualizeHighLevelMemory() = 0;
        virtual std::string visualizeDetailedMemory(size_t activeTicks, size_t idleTicks) = 0;

        virtual size_t getCapacity() const = 0;
        virtual size_t getAllocatedSize() const = 0;
        virtual size_t getFreeSize() const = 0;

		virtual uint16_t readMemory(void* handle, uintptr_t address) = 0;
		virtual void writeMemory(void* handle, uintptr_t address, uint16_t value) = 0;
};