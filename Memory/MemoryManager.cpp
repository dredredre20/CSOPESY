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

// Tries to give a process its memory allocation (virtual address space
// registration
bool MemoryManager::tryAdmitProcess(const std::shared_ptr<Process>& process) {
	std::lock_guard<std::mutex> lock(admitMutex);
    
    if (!process || !allocator) {
        return true;
    }

    if (process->isMemoryAllocated()) {
        return true;
    }

    if (process->getMemoryRequirement() <= 0) {
        return true;
    }

    void* block = allocator->allocate(process);

    if (block == nullptr) {
        return false;
    }

    process->setMemoryAllocatedBlock(block);
	process->setMemoryAllocator(allocator.get());
    memoryResidentProcesses.push_back(process);
    return true;
}

void MemoryManager::releaseProcessMemory(const std::shared_ptr<Process>& process) {
    std::lock_guard<std::mutex> lock(admitMutex);
    
    if (!process || !process->isMemoryAllocated() || !allocator) {
        return;
    }

    allocator->deallocate(process->getMemoryAllocatedBlock());
    process->clearMemoryAllocation();

    memoryResidentProcesses.erase(
        std::remove_if(memoryResidentProcesses.begin(), memoryResidentProcesses.end(),
            [&](const std::shared_ptr<Process>& p) { return p == process; }),
        memoryResidentProcesses.end());
}

MemorySnapshot MemoryManager::getMemorySnapshot() const {
	return allocator->getMemorySnapshot();
}