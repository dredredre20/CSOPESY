#pragma once
#include "../Config.hpp"
#include "../Process/Process.hpp"
#include <mutex>
#include <vector>
#include <thread>
#include <map>

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

	std::mutex queueMutex;

private:
	std::vector<std::thread> cpuThreads;
};