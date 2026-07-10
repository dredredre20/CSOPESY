#include "FlatMemoryAllocator.hpp"
#include <algorithm>
#include <limits>

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximum_size, size_t fixed_allocation_size) 
    : maximumSize(maximum_size), allocatedSize(0), fixedAllocationSize(fixed_allocation_size) {
    memory.resize(maximumSize); 
    initializeMemory();
}

void FlatMemoryAllocator::initializeMemory() {
    std::fill(memory.begin(), memory.end(), '.'); // '.' represents unallocated memory
    for (size_t i = 0; i < maximumSize; ++i) {
        allocationMap[i] = false;
    }
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const {
    // check if the memory block is large enough
    return (index + size <= maximumSize);
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
    // Mark the memory block as allocated
    for (size_t i = index; i < index + size; ++i) {
            allocationMap[i] = true;
    }

    allocationSizes[index] = size;
    allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index) {
    auto it = allocationSizes.find(index);
    if (it == allocationSizes.end()) {
        return;
    }

    size_t size = it->second;
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = false;
    }

    allocationSizes.erase(it);
    if (allocatedSize >= size) {
        allocatedSize -= size;
    } else {
        allocatedSize = 0;
    }
}

void* FlatMemoryAllocator::allocate(size_t size) {
    const size_t blockSize = (fixedAllocationSize > 0) ? fixedAllocationSize : size;

    if (blockSize == 0) {
        return nullptr;
    }

    // Find the first available block that can accommodate the process
    for (size_t i = 0; i <= maximumSize - blockSize; ++i) {
        if (!allocationMap[i] && canAllocateAt(i, blockSize)) {
            allocateAt(i, blockSize);
            return &memory[i];
        }
    }

    return nullptr;
}

void FlatMemoryAllocator::deallocate(void* ptr) {
    // Find the index of the memory block to deallocate
    size_t index = static_cast<char*>(ptr) - &memory[0];

    if (allocationMap[index]) {
        deallocateAt(index);
    }
}

std::string FlatMemoryAllocator::visualizeMemory() {
    return std::string(memory.begin(), memory.end());
}

size_t FlatMemoryAllocator::getCapacity() const {
    return maximumSize;
}

size_t FlatMemoryAllocator::getAllocatedSize() const {
    return allocatedSize;
}

size_t FlatMemoryAllocator::getFreeSize() const {
    return maximumSize - allocatedSize;
}

size_t FlatMemoryAllocator::getAllocationStart(const void* ptr) const {
    if (ptr == nullptr) {
        return std::numeric_limits<size_t>::max();
    }

    const char* begin = memory.data();
    const char* pos = static_cast<const char*>(ptr);
    if (pos < begin || pos >= begin + memory.size()) {
        return std::numeric_limits<size_t>::max();
    }

    size_t index = static_cast<size_t>(pos - begin);
    auto it = allocationMap.find(index);
    if (it == allocationMap.end() || !it->second) {
        return std::numeric_limits<size_t>::max();
    }

    return index;
}