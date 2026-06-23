#pragma once
#include <string>

struct Config {
	int numCPU = 0;
	std::string scheduler = ""; // fcfs or rr
	int quantumCycles = 0;
	int batchFreq = 0;
	int minIns = 0;
	int maxIns = 0;
	int delayPerExec = 0;
};