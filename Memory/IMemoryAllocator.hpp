#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <cstddef>

class IMemoryAllocator {
    public:
        virtual ~IMemoryAllocator() = default;
        enum MemoryAllocatorType {
            FLAT_MEMORY_ALLOCATOR,
            PAGING,
        };

        virtual void* allocate(size_t size) = 0;
        virtual void deallocate(void* ptr) = 0;
        virtual std::string visualizeMemory() = 0;

        // not in sir's example, but it is placed in ours
        virtual size_t getCapacity() const = 0;
        virtual size_t getAllocatedSize() const = 0;
        virtual size_t getFreeSize() const = 0;
        virtual size_t getAllocationStart(const void* ptr) const = 0;
    protected:
        MemoryAllocatorType memoryAllocatorType;

        struct MemoryBlock {
            size_t start;
            size_t size;

            bool operator<(const MemoryBlock& other) const {
                return start < other.start;
            }
        };

        size_t maximumSize;
        size_t currentAllocatedSize;

};