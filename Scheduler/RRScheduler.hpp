#pragma once
#include "../Scheduler/Scheduler.hpp"

class RRScheduler : public Scheduler {
    public:
        RRScheduler(int timeSlice) : timeSlice(timeSlice) {}
    protected:
        void runCycle(int coreId) override;
    private:
        int timeSlice;
};