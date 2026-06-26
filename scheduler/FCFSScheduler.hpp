#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include "../Process/Process.hpp"
#include "../Process/ProcessResolver.hpp"
#include "../Scheduler/Scheduler.hpp"

class FCFSScheduler : public Scheduler, public ProcessResolver {
private:
    Process* getProcessOnCore(int coreId) override {
        auto it = runningProcesses.find(coreId);
        return (it != runningProcesses.end()) ? it->second.get() : nullptr;
    }
protected:
    void runCycle(int coreId) override;
};