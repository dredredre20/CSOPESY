#pragma once
#include "../Scheduler/Scheduler.hpp"
#include "../Process/ProcessResolver.hpp"

class RRScheduler : public Scheduler, public ProcessResolver {
public:
    RRScheduler(int timeSlice) : timeSlice(timeSlice) {}
protected:
    void runCycle(int coreId) override;
private:
    Process* getProcessOnCore(int coreId) override {
        auto it = runningProcesses.find(coreId);
        return (it != runningProcesses.end()) ? it->second.get() : nullptr;
    }
    int timeSlice;
};