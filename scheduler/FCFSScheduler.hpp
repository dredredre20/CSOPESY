#pragma once
#include "../Scheduler/Scheduler.hpp"

class FCFSScheduler : public Scheduler {
    protected:
        void runCycle(int coreId) override;
};