#pragma once
#include "../Config.hpp"
#include "../process/Process.hpp"
#include <mutex>
#include <vector>
#include <thread>
#include <map>

class Scheduler {
public:
	virtual ~Scheduler() = default;

	void initialize(const Config& config);
	void addProcess(const Process& process);
	void start();
	void stop();

	void screenLs();
	void reportUtil();
	Process* findProcessByName(const std::string& name);

protected:
	virtual void runCycle() = 0;
	static std::string getTimestamp();

	Config config;
	bool running = false;
	int nextCore = 0;

	std::vector<std::vector<Process>> processQueues;
	std::map<int, Process*> runningProcesses;
	std::vector<Process> finishedProcesses;
	std::mutex queueMutex;

private:
	std::vector<std::thread> cpuThreads;
};