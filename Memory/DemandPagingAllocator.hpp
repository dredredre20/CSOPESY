#pragma once
#include "IMemoryAllocator.hpp"
#include <vector>
#include <unordered_map>
#include <deque>
#include <string>
#include <cstddef>


class DemandPagingAllocator : public IMemoryAllocator {
private:
    struct PTE {
        bool valid = false; // false mean page is in the backing store
        size_t frameNumber = 0; // only meaningful if valid == true
    };

    // Allocation record of a process
    struct AllocationRecord {
        long long allocationId; // identifier for the allocation
        std::shared_ptr<Process> process; // the process that owns this allocation
        size_t numPages; // how many pages the process occupies
        std::vector<PTE> pageTable; // tracks whether each page is in memory, and which frame it's in if so
    };
    
    size_t frameSize; // config's mem-per-frame
    size_t numFrames; // maximumSize/frameSize
    size_t maximumSize; // physical memory size in bytes
    size_t allocatedSize; // bytes currently in the physical memory 
    long long nextAllocationId; // counter to generate unique allocation IDs

    std::vector<char> physicalMemory; 
    std::vector<int> frameOwner; // frameOwner[frame] = which allocationId owns it, -1 = free
    std::vector<size_t> framePage; // framePage[frame] = which virtual page number sits there

    std::deque<size_t> fifoQueue; // tracks order frames were loaded in -> used for FIFO eviction
    std::deque<size_t> freeFrameList;

    std::unordered_map<long long, AllocationRecord> allocations; // for process lookup by internal allocation ID
    std::unordered_map<void*, long long> pointerToAllocationId;

    std::string backingStoreFile = "csopesy-backing-store.txt";

    // Picks a victim frame (oldest resident, FIFO) and evicts it. 
    size_t evictOnePage();

    // Copies a page's data into a frame and updates all the
    // bookkeeping (PTE, frameOwner, fifoQueue, etc.)
    void loadPageIntoFrame(long long allocationId, size_t pageNum, size_t frameIndex);

    // Appends one page's info as a text block to the shared backing store file.
    void writePageToBackingStore(const AllocationRecord& record, size_t pageNum);

    // Rewrites the backing store file without the given process/page's entry,
    void removePageFromBackingStore(long long allocationId, size_t pageNum);

	// Validates a single virtual address, faults in the page if necessary, and returns the corresponding physical address.
	size_t resolvePhysicalAddress(void* ptr, long long allocationId, uintptr_t virtualAdrr);

    size_t numPagedIn = 0;  // total count of "page brought INTO memory" events
    size_t numPagedOut = 0; // total count of "page evicted OUT to backing store" events

    mutable std::mutex allocatorMutex;

public:
    DemandPagingAllocator(size_t maximumSize, size_t frameSize);
    ~DemandPagingAllocator() override;

    void* allocate(std::shared_ptr<Process> Process) override;

    void deallocate(void* ptr) override;
    std::string visualizeMemory() override;

    size_t getCapacity() const override;      
    size_t getAllocatedSize() const override; 
    size_t getFreeSize() const override;     

    // Function for demand paging: trigger's page fault
    void accessPage(void* ptr, size_t pageNum);

	uint16_t readMemory(void* handle, uintptr_t address) override;
	void writeMemory(void* handle, uintptr_t address, uint16_t value) override;

    size_t getNumPagedIn() const { return numPagedIn; }
    size_t getNumPagedOut() const { return numPagedOut; }
};