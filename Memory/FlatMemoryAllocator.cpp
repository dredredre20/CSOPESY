#include "FlatMemoryAllocator.hpp"
#include <algorithm>

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximum_size) 
    : maximumSize(maximum_size), allocatedSize(0) {
    memory.reserve(maximumSize); 
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
    
    allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index) {
    // Mark the memory block as deallocated 
    allocationMap[index] = false;
}

void* FlatMemoryAllocator::allocate(size_t size) {
    // Find the first available blocks that can accommodate the process
    for (size_t i = 0; i <= maximumSize - size; i++) {
        if (!allocationMap[i] && canAllocateAt(i, size)) {
            allocateAt(i, size);
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