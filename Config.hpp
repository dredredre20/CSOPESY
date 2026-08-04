#pragma once
#include <string>
#include <stdexcept>

struct Config {
	int numCPU = 0;
	std::string scheduler = ""; // fcfs or rr
	int quantumCycles = 0;
	int batchFreq = 0;
	int minIns = 0;
	int maxIns = 0;
	int delayPerExec = 0;

	int maxOverallMem = 0;
	int memPerFrame = 0;
	int minMemPerProc = 0;
	int maxMemPerProc = 0;

	static constexpr int MEM_MIN = 64; // 2^6
	static constexpr int MEM_MAX = 65536; // 2^16

	static bool isPowerOfTwo(int n) {
		return n > 0 && (n & (n - 1)) == 0;
	}

	static bool isInMemRange(int n) {
		return n >= MEM_MIN && n <= MEM_MAX;
	}

	// Validation for memory constraints
	void validateMemory() const {
		const std::pair<std::string, int> fields[] = {
			{ "max-overall-mem", maxOverallMem },
			{ "mem-per-frame", memPerFrame },
			{ "min-mem-per-proc", minMemPerProc },
			{ "max-mem-per-proc", maxMemPerProc }
		};

		for (const auto& [name, value] : fields) {
			if (!isPowerOfTwo(value))
				throw std::runtime_error("Error: " + name + " must be a power of 2.");
		}

		if (memPerFrame > maxOverallMem)
			throw std::runtime_error("Error: mem-per-frame cannot exceed max-overall-mem.");

		if (minMemPerProc > maxMemPerProc)
			throw std::runtime_error("Error: min-mem-per-proc cannot exceed max-mem-per-proc.");
	}

	void validateProcessMemSize(int processMemSize) const {
		if (processMemSize < minMemPerProc || processMemSize > maxMemPerProc) {
			throw std::runtime_error("Error: Invalid memory allocation");
		}

		if (!isPowerOfTwo(processMemSize))
			throw std::runtime_error("Error: Invalid memory allocation");

		if (minMemPerProc < MEM_MIN)
			throw std::runtime_error("Error: min-mem-per-proc must be at least 64");
	}

	int getNumFrame() const { return maxOverallMem / memPerFrame; }
};