#pragma once
#include "Scheduler.hpp"

class RRScheduler : public Scheduler {
	void runCycle() override;
};