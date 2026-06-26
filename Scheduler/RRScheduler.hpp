#pragma once
#include "../Scheduler/Scheduler.hpp"
#include "../process/ProcessResolver.hpp"

class RRScheduler : public Scheduler, public ProcessResolver {

// timeslice is inherited from config
public: 
    RRScheduler(int timeSlice) : timeSlice(timeSlice) {}

protected:
    void runCycle() override;
    //Process* getProcessOnCore(int coreId) override;

private:
    Process* getProcessOnCore(int coreId) override {
        return runningProcesses[coreId];
    }
    int timeSlice;
};