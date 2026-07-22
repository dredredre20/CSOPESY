#pragma once
#include "IMemoryAllocator.hpp"
#include <map>
#include <vector>

class PagingAllocator : public IMemoryAllocator {
    private:
        size_t frameSize;
        size_t numFrames;

        // frameOwner[i] == -1 means frame i is free; otherwise it holds
        // the allocation id currently occupying that frame.
        std::vector<long long> frameOwner;

        struct AllocationRecord {
            char* buffer;
            size_t sizeInBytes;
            std::vector<MemoryBlock> frameBlocks;
        };

        std::map<long long, AllocationRecord> allocations;   // keyed by allocation id
        std::map<void*, long long> pointerToAllocationId;    // reverse lookup
        long long nextAllocationId;

        std::vector<MemoryBlock> findFreeFrames(size_t framesNeeded) const;

        static size_t totalFrames(const std::vector<MemoryBlock>& blocks);

    public:
        PagingAllocator(size_t maximum_size, size_t frame_size);
        ~PagingAllocator() override;

        void* allocate(size_t size) override;
        void deallocate(void* ptr) override;
        std::string visualizeMemory() override;
        size_t getCapacity() const override;
        size_t getAllocatedSize() const override;
        size_t getFreeSize() const override;
        size_t getAllocationStart(const void* ptr) const override;
};