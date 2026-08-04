#include "DemandPagingAllocator.hpp"
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <cmath>
#include <mutex>

DemandPagingAllocator::DemandPagingAllocator(size_t maximumSize, size_t frameSize)
    : frameSize(frameSize),
      numFrames(maximumSize / frameSize),
      maximumSize(maximumSize),
      allocatedSize(0),
      nextAllocationId(0)
{
    physicalMemory.resize(numFrames * frameSize); 
    frameOwner.assign(numFrames, -1); // -1 = every frame starts free
    framePage.assign(numFrames, 0);

    // Initialize the free frame list
    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(i);
    }
}

DemandPagingAllocator::~DemandPagingAllocator() {
    for (auto& [ptr, id] : pointerToAllocationId) {
        delete[] static_cast<char*>(ptr);
    }
}

// This function does not load any pages into memory yet 
// Loading pages into memory happens when accessPage() is 
// called and triggers an actual page fault.
void* DemandPagingAllocator::allocate(std::shared_ptr<Process> process) {
    {
        std::lock_guard<std::recursive_mutex> lock(allocatorMutex);
        
        size_t size = process -> getMemoryRequirement();

        if (size == 0 || size > maximumSize) {
            return nullptr; 
        }

        size_t numPages = (size + frameSize - 1) / frameSize; // calculate number of pages needed
        long long allocationId = nextAllocationId++;

        // Create an allocation record for the process
        AllocationRecord record;
        record.allocationId = allocationId;
        record.process = process;
        record.numPages = numPages;
        record.pageTable.assign(numPages, PTE{}); // all pages start as "not in memory" (invalid)

        allocations[allocationId] = record;

        void* handle = new char[1];
        pointerToAllocationId[handle] = allocationId;
        return handle;
    }
}

// Called whenever a process's instruction touches a specific virtual page.
// This is where the actual page fault handling happens.
void DemandPagingAllocator::accessPage(void* ptr, size_t pageNum) {
    {
        std::lock_guard<std::recursive_mutex> lock(allocatorMutex);

        auto it = pointerToAllocationId.find(ptr);
        if (it == pointerToAllocationId.end()) {
            throw std::invalid_argument("Invalid process handle.");
        }

        long long allocationId = it->second;
        auto& record = allocations.at(allocationId);

        if (pageNum >= record.pageTable.size()) {
            throw std::out_of_range("Access violation: page index out of process's allocated range.");
        }

        PTE& pte = record.pageTable[pageNum];
        if (pte.valid) {
            return; // page is already in memory
        }

        // Page Fault
        size_t frameIndex;
        if (!freeFrameList.empty()) {
            // There's a free frame
            frameIndex = freeFrameList.front();
            freeFrameList.pop_front();
        } else {
            // Free frame list is empty - evict a page to backing store
            frameIndex = evictOnePage();
        }

        // Load the page into the frame
        loadPageIntoFrame(allocationId, pageNum, frameIndex);
    }
}

// Pick the oldest page and transfer it to the backing store
// No need for mutex here, since this process happens independent from other shared states
size_t DemandPagingAllocator::evictOnePage() {
    size_t victimFrame = fifoQueue.front(); 
    fifoQueue.pop_front();

    long long victimAllocationId = frameOwner[victimFrame];
    size_t victimPageNum = framePage[victimFrame];

    auto& victimRecord = allocations.at(victimAllocationId);
    PTE& victimPTE = victimRecord.pageTable[victimPageNum];

    // Save page info to the backing store
    writePageToBackingStore(victimRecord, victimPageNum);
    numPagedOut++; // tally: one more page pushed OUT to backing store

    victimPTE.valid = false;      // this page is no longer in memory
    frameOwner[victimFrame] = -1; // frame is now unowned
    allocatedSize -= frameSize;   // physical memory usage just went down

    return victimFrame; // hand back the now-free frame to whoever needs it
}

// Actually moves a page into physical memory and updates all bookkeeping.
// No need for mutex here, since this process happens independent from other shared states
void DemandPagingAllocator::loadPageIntoFrame(long long allocationId, size_t pageNum, size_t frameIndex) {
    auto& record = allocations.at(allocationId);

    removePageFromBackingStore(allocationId, pageNum);
    numPagedIn++; // tally: one more page brought in from backing store

    frameOwner[frameIndex] = static_cast<int>(allocationId);
    framePage[frameIndex] = pageNum;

    PTE& pte = record.pageTable[pageNum];
    pte.valid = true;
    pte.frameNumber = frameIndex;

    fifoQueue.push_back(frameIndex);
    allocatedSize += frameSize; // update physical memory usage
}

// Writes page metadata as a text block into the shared backing store file.
// No need for mutex here, since this process happens independent from other shared states
void DemandPagingAllocator::writePageToBackingStore(const AllocationRecord& record, size_t pageNum) {
    std::ofstream file(backingStoreFile, std::ios::app); // append, don't overwrite existing entries
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open backing store file for writing.");
    }

    file << "PID: " << record.process -> getPID() << "\n";
    file << "Name: " << record.process -> getName() << "\n";
    file << "Command Counter: " << record.process -> getCommandCounter() << "\n";
    file << "Page Number: " << pageNum << "\n";
    file << "Number of Pages: " << record.numPages << "\n";
    file << "Page Size: " << frameSize << "\n";
    file << "---\n"; // separator so we can tell entries apart when reading back
}

// Removes one specific process/page's entry from the backing store file.
// No need for mutex here, since this process happens independent from other shared states
void DemandPagingAllocator::removePageFromBackingStore(long long allocationId, size_t pageNum) {
    auto& record = allocations.at(allocationId);

    std::ifstream inFile(backingStoreFile);
    if (!inFile.is_open()) {
        return; 
    }

    std::ostringstream updatedContents; // will hold everything except the removed block
    std::string line;
    std::vector<std::string> currentBlock; // lines belonging to the entry that is currently being read
    bool skipBlock = false; // true if this block matches the entry we want to remove

    while (std::getline(inFile, line)) {
        if (line == "---") {
            // End of one block -- decide whether to keep it or drop it.
            if (!skipBlock) {
                for (const auto& blockLine : currentBlock) {
                    updatedContents << blockLine << "\n";
                }
                updatedContents << "---\n";
            }
            currentBlock.clear();
            skipBlock = false;
            continue;
        }

        currentBlock.push_back(line);

        // If this block's page number matches AND its PID matches, mark it for removal.
        if (line == ("Page Number: " + std::to_string(pageNum))) {
            bool pidMatches = std::any_of(currentBlock.begin(), currentBlock.end(),
                [&](const std::string& l) { return l == ("PID: " + std::to_string(record.process -> getPID())); });
            if (pidMatches) {
                skipBlock = true;
            }
        }
    }
    inFile.close();

    // Overwrite the file with everything except the removed block.
    std::ofstream outFile(backingStoreFile, std::ios::trunc);
    outFile << updatedContents.str();
}

// Frees all of a process's memory, both resident frames and any pages
// still sitting in the backing store.
void DemandPagingAllocator::deallocate(void* ptr) {
    {
        std::lock_guard<std::recursive_mutex> lock(allocatorMutex);

        auto it = pointerToAllocationId.find(ptr);
        if (it == pointerToAllocationId.end()) {
            throw std::invalid_argument("Pointer does not correspond to an active allocation.");
        }

        long long allocationId = it->second;
        auto& record = allocations.at(allocationId);

        for (size_t p = 0; p < record.pageTable.size(); ++p) {
            PTE& pte = record.pageTable[p];
            if (pte.valid) {
                // Page was in memory -- free its frame.
                frameOwner[pte.frameNumber] = -1;
                freeFrameList.push_back(pte.frameNumber);
                allocatedSize -= frameSize;

                // Remove it from the FIFO queue too, so eviction doesn't
                // later try to reference a frame that's already been freed.
                auto fifoIt = std::find(fifoQueue.begin(), fifoQueue.end(), pte.frameNumber);
                if (fifoIt != fifoQueue.end()) fifoQueue.erase(fifoIt);
            } else {
                // Page was swapped out -- clean up its backing store entry too.
                removePageFromBackingStore(allocationId, p);
            }
        }

        allocations.erase(allocationId);
        delete[] static_cast<char*>(ptr); // free synthetic handle allocated earlier
        pointerToAllocationId.erase(it);
    }
}

MemorySnapshot DemandPagingAllocator::getMemorySnapshot() {
    {
        std::lock_guard<std::recursive_mutex> lock(allocatorMutex);
        MemorySnapshot snap;
        snap.capacitySize = maximumSize;
        snap.allocatedSize = allocatedSize;
        for (const auto& [id, record] : allocations) {
            size_t resident = 0;
            for (const auto& pte : record.pageTable) if (pte.valid) resident += frameSize;
            if (resident > 0) snap.processResidentBytes.emplace_back(record.process->getName(), resident);
        }
        snap.numPagedIn = numPagedIn;
        snap.numPagedOut = numPagedOut;

        return snap;
    }
}

size_t DemandPagingAllocator::getCapacity() const {
    return maximumSize; 
}

size_t DemandPagingAllocator::getAllocatedSize() const {
    return allocatedSize; 
}

size_t DemandPagingAllocator::getFreeSize() const {
    return maximumSize - allocatedSize;
}

// Resolves a virtual address to a physical address, ensuring the page is in memory.
size_t DemandPagingAllocator::resolvePhysicalAddress(void* ptr, long long allocationId, uintptr_t virtualAddr){
    auto& record = allocations.at(allocationId);
	size_t procSize = record.process->getMemoryRequirement();

	// Check if the virtual address is within the bounds of the process's allocated memory
	if (virtualAddr >= procSize) {
		throw MemoryAccessViolationException(virtualAddr);
	}
    
    // calculate the page number and offset to properly resolve the size of the address depending on the frame size
	size_t pageNum = virtualAddr / frameSize;
	size_t offset = virtualAddr % frameSize;

	accessPage(ptr, pageNum); // ensure the page is in memory)

	PTE& pte = record.pageTable[pageNum];
	return pte.frameNumber * frameSize + offset; // physical address
}


// Reads a 16-bit value from the process's virtual address space, handling page faults as needed.
uint16_t DemandPagingAllocator::readMemory(void* handle, uintptr_t address) {
    {
        std::lock_guard<std::recursive_mutex> lock(allocatorMutex);

        auto it = pointerToAllocationId.find(handle);

        if (it == pointerToAllocationId.end()) {
            throw std::invalid_argument("Invalid process handle.");
        }

        // Get the allocation ID and corresponding record for the process
        long long allocationId = it->second;
        auto& record = allocations.at(allocationId);
        size_t procSize = record.process->getMemoryRequirement();

        if (address + 1 >= procSize) { // check if address still fits within the process's allocated space
            throw MemoryAccessViolationException(address);
        }

        // calculate physical addresses for the two bytes to read
        size_t physLow = resolvePhysicalAddress(handle, allocationId, address);
        size_t physHigh = resolvePhysicalAddress(handle, allocationId, address + 1);

        // Fetch bytes and combine them into a 16-bit value for better type safety and to avoid sign extension issues
        uint8_t lowByte = static_cast<uint8_t>(physicalMemory[physLow]);
        uint8_t highByte = static_cast<uint8_t>(physicalMemory[physHigh]);

        return static_cast<uint16_t>(lowByte) | (static_cast<uint16_t>(highByte) << 8);
    }
}


void DemandPagingAllocator::writeMemory(void* handle, uintptr_t address, uint16_t value) {
    {
        std::lock_guard<std::recursive_mutex> lock(allocatorMutex);
        auto it = pointerToAllocationId.find(handle);
        if (it == pointerToAllocationId.end()) {
            throw std::invalid_argument("Invalid process handle.");
        }

        long long allocationId = it->second;
        auto& record = allocations.at(allocationId);
        size_t procSize = record.process->getMemoryRequirement();

        if (address + 1 >= procSize) {
            throw MemoryAccessViolationException(address);
        }

        // calculate physical addresses for the two bytes to read
        size_t physLow = resolvePhysicalAddress(handle, allocationId, address);
        size_t physHigh = resolvePhysicalAddress(handle, allocationId, address + 1);

        // store the low and high bytes of the 16-bit value directly into the calculated physical addresses
        // FF is used to mask the 8 bits since physicalMemory is a vector of char
        physicalMemory[physLow] = static_cast<char>(value & 0xFF);
        physicalMemory[physHigh] = static_cast<char>((value >> 8) & 0xFF);
    }
}