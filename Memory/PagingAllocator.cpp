#include "PagingAllocator.hpp"
#include <algorithm>
#include <sstream>
#include <stdexcept>

PagingAllocator::PagingAllocator(size_t maximum_size, size_t frame_size)
    : frameSize(frame_size == 0 ? maximum_size : frame_size),
      numFrames(frame_size == 0 ? 1 : maximum_size / frame_size),
      nextAllocationId(0)
{
    memoryAllocatorType = PAGING;
    maximumSize = maximum_size;
    currentAllocatedSize = 0;
    frameOwner.assign(numFrames, -1);
}

PagingAllocator::~PagingAllocator() {
    for (auto& [id, record] : allocations) {
        delete[] record.buffer;
    }
}

size_t PagingAllocator::totalFrames(const std::vector<MemoryBlock>& blocks) {
    size_t total = 0;
    for (const auto& block : blocks) {
        total += block.size;
    }
    return total;
}

std::vector<PagingAllocator::MemoryBlock> PagingAllocator::findFreeFrames(size_t framesNeeded) const {
    std::vector<MemoryBlock> freeBlocks;
    size_t collected = 0;
 
    size_t i = 0;
    while (i < numFrames && collected < framesNeeded) {
        // Skip occupied frames
        if (frameOwner[i] != -1) {
            ++i;
            continue;
        }
 
        // A free frame ha sbeen found! Start a loop from here and measure how long this free frame is
        size_t runStart = i;
        size_t runLength = 0;
        while (i < numFrames && frameOwner[i] == -1) {
            ++runLength;
            ++i;
        }
 
        // If more free frames than needed frames were found, reduce the target frames to the needed frames
        size_t needed = framesNeeded - collected;
        if (runLength > needed) {
            runLength = needed;
        }
 
        // add the collected free blocks of memory
        freeBlocks.push_back(MemoryBlock{runStart, runLength});

        // increment the current collected memory so the while loops knows whether to tuloy or stop
        collected += runLength;
    }
 
    // Edge case, not enough free frames
    if (collected < framesNeeded) {
        return {};
    }
 
    return freeBlocks;
}

void* PagingAllocator::allocate(size_t size) {
    if (size == 0 || size > maximumSize) {
        return nullptr;
    }

    size_t pagesNeeded = (size + frameSize - 1) / frameSize;
    std::vector<MemoryBlock> freeBlocks = findFreeFrames(pagesNeeded);
    if (freeBlocks.empty() || totalFrames(freeBlocks) < pagesNeeded) {
        return nullptr; // Not enough free frames overall, even if scattered.
    }

    long long allocationId = nextAllocationId++;
    for (const auto& block : freeBlocks) {
        for (size_t frameIndex = block.start; frameIndex < block.start + block.size; ++frameIndex) {
            frameOwner[frameIndex] = allocationId;
        }
    }

    // grabbing scattered locations of process and compiling it into the AllocationRecord object
    size_t roundedSize = pagesNeeded * frameSize;
    char* buffer = new char[roundedSize];

    AllocationRecord record{buffer, roundedSize, freeBlocks};
    allocations[allocationId] = record;
    pointerToAllocationId[static_cast<void*>(buffer)] = allocationId;

    currentAllocatedSize += roundedSize;
    return static_cast<void*>(buffer);
}

void PagingAllocator::deallocate(void* ptr) {
    if (ptr == nullptr) {
        return;
    }

    auto ptrIt = pointerToAllocationId.find(ptr);
    if (ptrIt == pointerToAllocationId.end()) {
        throw std::invalid_argument("Pointer does not correspond to an active allocation.");
    }

    long long allocationId = ptrIt->second;
    auto& record = allocations.at(allocationId);

    for (const auto& block : record.frameBlocks) {
        for (size_t frameIndex = block.start; frameIndex < block.start + block.size; ++frameIndex) {
            frameOwner[frameIndex] = -1;
        }
    }

    currentAllocatedSize -= record.sizeInBytes;
    delete[] record.buffer;

    allocations.erase(allocationId);
    pointerToAllocationId.erase(ptrIt);
}

std::string PagingAllocator::visualizeMemory() {
    std::ostringstream oss;
    oss << "Paging Allocator [" << currentAllocatedSize << "/" << maximumSize
        << " bytes used, " << numFrames << " frames of " << frameSize << " bytes each]\n";

    for (size_t i = 0; i < numFrames; ++i) {
        oss << "Frame " << i << ": ";
        if (frameOwner[i] == -1) {
            oss << "free";
        } else {
            oss << "allocation #" << frameOwner[i];
        }
        oss << "\n";
    }

    return oss.str();
}

size_t PagingAllocator::getCapacity() const {
    return maximumSize;
}

size_t PagingAllocator::getAllocatedSize() const {
    return currentAllocatedSize;
}

size_t PagingAllocator::getFreeSize() const {
    return maximumSize - currentAllocatedSize;
}

size_t PagingAllocator::getAllocationStart(const void* ptr) const {
    auto it = pointerToAllocationId.find(const_cast<void*>(ptr));
    if (it == pointerToAllocationId.end()) {
        throw std::invalid_argument("Pointer does not correspond to an active allocation.");
    }
 
    const AllocationRecord& record = allocations.at(it->second);
 
    size_t firstFrame = record.frameBlocks.front().start;
    for (const auto& block : record.frameBlocks) {
        firstFrame = std::min(firstFrame, block.start);
    }
 
    return firstFrame * frameSize;
}