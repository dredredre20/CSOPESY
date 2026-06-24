#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include "../process/Process.hpp"
#include "../process/ProcessResolver.hpp"
#include "../Scheduler/Scheduler.hpp"

class FCFSScheduler : public Scheduler, public ProcessResolver {
private:
    Process* getProcessOnCore(int coreId) override {
        return runningProcesses[coreId];
    }

protected:
    void runCycle() override;
};