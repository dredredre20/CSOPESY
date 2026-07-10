#pragma once
#include "IMemoryAllocator.hpp"

class FlatMemoryAllocator : public IMemoryAllocator {
    private:
        size_t maximumSize;
        size_t allocatedSize;
        size_t fixedAllocationSize;
        std::vector<char> memory;
        std::unordered_map<size_t, bool> allocationMap;
        std::unordered_map<size_t, size_t> allocationSizes;

        void initializeMemory();
        bool canAllocateAt(size_t index, size_t size) const;
        void allocateAt(size_t index, size_t size);
        void deallocateAt(size_t index);

    public:
        FlatMemoryAllocator(size_t maximum_size, size_t fixed_allocation_size);
        
        void* allocate(size_t size) override;
        void deallocate(void* ptr) override;
        std::string visualizeMemory() override;
        size_t getCapacity() const override;
        size_t getAllocatedSize() const override;
        size_t getFreeSize() const override;
        size_t getAllocationStart(const void* ptr) const override;
};