#pragma once
#include "IMemoryAllocator.hpp"
#include "../process/Process.hpp"
#include <memory>

class MemoryManager {
public:
	static MemoryManager* getInstance();

	void initialize(size_t maxOverallMem, size_t memPerFrame);

	void* allocate(std::shared_ptr<Process> process);

	void deallocate(void* handle);

	size_t getFreeSize() const;
	size_t getAllocatedSize() const;
	size_t getCapacity() const;

	std::string visualizeHighLevelMemory() const;
	std::string visualizeDetailedMemory(size_t active, size_t idle) const;

	IMemoryAllocator* getAllocator() const { return allocator.get(); }

private:
	MemoryManager() = default;
	static MemoryManager* instance;
	std::shared_ptr<IMemoryAllocator> allocator;
};