#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <cstddef>

class IMemoryAllocator {
    public:
        virtual ~IMemoryAllocator() = default;
        virtual void* allocate(size_t size) = 0;
        virtual void deallocate(void* ptr) = 0;
        virtual std::string visualizeMemory() = 0;
        virtual size_t getCapacity() const = 0;
        virtual size_t getAllocatedSize() const = 0;
        virtual size_t getFreeSize() const = 0;
        virtual size_t getAllocationStart(const void* ptr) const = 0;
};