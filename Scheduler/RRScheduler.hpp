#pragma once
#include "Scheduler.hpp"

class RRScheduler : public Scheduler {
    // timeslice is inherited from config
public: 
    RRScheduler(int timeSlice) : timeSlice(timeSlice) {}

protected:
    void runCycle() override;

private:
    int timeSlice;
};