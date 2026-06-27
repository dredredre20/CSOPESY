#include "SleepCommand.hpp"
#include "../Process/Process.hpp"
#include <sstream>
#include <chrono>
#include <thread>

void SleepCommand::execute(int coreID, Process& process) {
    process.setSleepTicks(ticks);
    process.setSleeping(true);
}