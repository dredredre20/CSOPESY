#include "MemoryManager.hpp"
#include "DemandPagingAllocator.hpp"

MemoryManager* MemoryManager::instance = nullptr;

MemoryManager* MemoryManager::getInstance() {
	if (!instance) {
		instance = new MemoryManager();
	}
	return instance;
}

void MemoryManager::initialize(size_t maxOverallMem, size_t memPerFrame) {
	allocator = std::static_pointer_cast<IMemoryAllocator>(
		std::make_shared<DemandPagingAllocator>(maxOverallMem, memPerFrame)
	);
}

void* MemoryManager::allocate(std::shared_ptr<Process> process) {
	return allocator->allocate(process);
}

void MemoryManager::deallocate(void* handle) {
	allocator->deallocate(handle);
}

size_t MemoryManager::getFreeSize() const {
	return allocator->getFreeSize();
}

size_t MemoryManager::getAllocatedSize() const {
	return allocator->getAllocatedSize();
}

size_t MemoryManager::getCapacity() const {
	return allocator->getCapacity();
}

// Visualization remove from allocator and put in memory
std::string MemoryManager::visualizeHighLevelMemory() const {
	return allocator->visualizeHighLevelMemory();
}

std::string MemoryManager::visualizeDetailedMemory(size_t active, size_t idle) const {
	return allocator->visualizeDetailedMemory(active, idle);
}