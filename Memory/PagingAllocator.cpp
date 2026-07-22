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

std::vector<size_t> PagingAllocator::findFreeFrames(size_t count) const {
    std::vector<size_t> free;
    free.reserve(count);
    for (size_t i = 0; i < numFrames && free.size() < count; ++i) {
        if (frameOwner[i] == -1) {
            free.push_back(i);
        }
    }
    return free;
}

void* PagingAllocator::allocate(size_t size) {
    if (size == 0 || size > maximumSize) {
        return nullptr;
    }

    size_t pagesNeeded = (size + frameSize - 1) / frameSize;
    std::vector<size_t> freeFrames = findFreeFrames(pagesNeeded);
    if (freeFrames.size() < pagesNeeded) {
        return nullptr; // Not enough free frames overall, even if scattered.
    }

    long long allocationId = nextAllocationId++;
    for (size_t frameIndex : freeFrames) {
        frameOwner[frameIndex] = allocationId;
    }

    // The frames backing this allocation can be scattered across physical
    // memory (that's the entire point of paging), but the caller still
    // needs one contiguous pointer to work with. We hand back a
    // page-aligned buffer sized for the request; the frame table above is
    // what tracks physical occupancy/fragmentation for accounting and
    // visualization.
    size_t roundedSize = pagesNeeded * frameSize;
    char* buffer = new char[roundedSize];

    AllocationRecord record{buffer, roundedSize, freeFrames};
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

    for (size_t frameIndex : record.frameIndices) {
        frameOwner[frameIndex] = -1;
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
    size_t firstFrame = *std::min_element(record.frameIndices.begin(), record.frameIndices.end());
    return firstFrame * frameSize;
}