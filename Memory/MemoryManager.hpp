#pragma once
#include "IMemoryAllocator.hpp"
#include "../Process/Process.hpp"
#include <memory>
#include <mutex>

class MemoryManager {
public:
	static MemoryManager* getInstance();

	void initialize(size_t maxOverallMem, size_t memPerFrame);

	void* allocate(std::shared_ptr<Process> process);

	void deallocate(void* handle);

	size_t getFreeSize() const;
	size_t getAllocatedSize() const;
	size_t getCapacity() const;

	MemorySnapshot getMemorySnapshot() const;

	std::vector<std::shared_ptr<Process>> memoryResidentProcesses;

	bool tryAdmitProcess(const std::shared_ptr<Process>& process);
	void releaseProcessMemory(const std::shared_ptr<Process>& process);

	IMemoryAllocator* getAllocator() const { return allocator.get(); }

private:
	MemoryManager() = default;
	static MemoryManager* instance;
	std::shared_ptr<IMemoryAllocator> allocator;

	mutable std::mutex admitMutex;
};