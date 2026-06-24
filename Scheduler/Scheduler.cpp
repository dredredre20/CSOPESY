#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "Scheduler.hpp"

using namespace std;

void Scheduler::initialize(const Config& cfg) {
    this->config = cfg;
    processQueues.resize(cfg.numCPU);
}

void Scheduler::addProcess(const Process& process) {
    lock_guard<mutex> lock(queueMutex);
    processQueues[nextCore].push_back(process);
	nextCore = (nextCore + 1) % config.numCPU;
}

void Scheduler::start() {
    running = true;
    for (int i = 0; i < config.numCPU; ++i) {
        cpuThreads.emplace_back(&Scheduler::runCycle, this);
    }
}

void Scheduler::stop() {
    cout << "Stopping scheduler...\n";
    running = false;
    for (auto& t : cpuThreads)
        if (t.joinable()) t.join();
    cpuThreads.clear();
}

string Scheduler::getTimestamp() {
	auto now = chrono::system_clock::now();
	time_t t = chrono::system_clock::to_time_t(now);
	tm tm = *std::localtime(&t);

	ostringstream oss;
	oss << put_time(&tm, "(%m/%d/%Y %I:%M:%S%p)");
	return oss.str();
}

void Scheduler::screenLs() {
    lock_guard<mutex> lock(queueMutex);
    std::string border = "--------------------------------";

    // Print current running processes
    cout << "\n";
    cout << border;
    cout << "\nRunning Processes:\n";
    if (runningProcesses.empty()) {
        cout << "\n";
    }
    else {
        for (const auto& [core, p] : runningProcesses) {
            cout << p->getName()
                << getTimestamp()
                << " Core:" << core
                << "   " << p->getCommandCounter()
                << "/" << p->getLinesOfCode() << "\n";
        }
    }

    // Print history log of finished processes
    cout << "\nFinished Processes:\n";
    if (finishedProcesses.empty()) {
        cout << "\n";
    }
    for (const auto& p : finishedProcesses) {
        cout << p.getName()
            << " (" << p.getCreationTimestamp() << ")"
            << " Finished   "
            << p.getLinesOfCode() << "/" << p.getLinesOfCode() << "\n";
    }
    cout << border;
    cout << "\n";
}

Process* Scheduler::findProcessByName(const std::string& name) {
	lock_guard<mutex> lock(queueMutex);
	for (const auto& [core, p] : runningProcesses) {
		if (p->getName() == name) {
			return p;
		}
	}
	for (const auto& p : finishedProcesses) {
		if (p.getName() == name) {
			return const_cast<Process*>(&p);
		}
	}
	return nullptr;
}