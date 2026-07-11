#pragma once
#include "../Config.hpp"
#include "../Process/Process.hpp"
#include "../Memory/IMemoryAllocator.hpp"
#include <mutex>
#include <vector>
#include <thread>
#include <map>
#include <memory>

class Scheduler {
public:
	virtual ~Scheduler() = default;

	void initialize(const Config& config);
	void addProcess(std::shared_ptr<Process> process);
	void start();
	void stop();

	void screenLs();
	void reportUtil();
	std::shared_ptr<Process> findProcessByName(const std::string& name);

protected:
    virtual void runCycle(int coreId) = 0;
	static std::string getTimestamp();

	Config config;
	bool running = false;
	int nextCore = 0;

	std::vector<std::vector<std::shared_ptr<Process>>> processQueues;
	std::map<int, std::shared_ptr<Process>> runningProcesses;	
	std::vector<std::shared_ptr<Process>> finishedProcesses;
	std::vector<std::shared_ptr<Process>> memoryResidentProcesses;

	std::unique_ptr<IMemoryAllocator> memoryAllocator;
	int quantumReportCounter = 0;
	std::mutex queueMutex;

	bool tryAdmitProcess(const std::shared_ptr<Process>& process);
	void releaseProcessMemory(const std::shared_ptr<Process>& process);
	void writeQuantumReport();

private:
	std::vector<std::thread> cpuThreads;
};