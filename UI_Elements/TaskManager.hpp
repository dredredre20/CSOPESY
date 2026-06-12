#pragma once

#include <string>
#include <vector>
#include <array>

struct ProcessInfo {
	int pid;
	std::string name;
	std::string state;
	float cpuUsage;
	float memoryUsage;
};

class TaskManager {
public:
	TaskManager();

	void drawPerformanceTab();
	void drawProcessesTab();
	//void sortProcesses(struct ImGuiTableSortSpecs* sortSpecs);

private:
	std::vector<ProcessInfo> processes;
	float currentCpuUsage = 35.0f;
	float currentMemUsage = 60.0f;
};