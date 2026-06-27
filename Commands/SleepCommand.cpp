#include "SleepCommand.hpp"
#include "../Process/Process.hpp"
#include <sstream>
#include <chrono>
#include <thread>

void SleepCommand::execute(int coreID, Process& process) {
    process.setSleepTicks(ticks);
    process.setSleeping(true);

    std::ostringstream oss;
    oss << " \"SLEEP " << static_cast<int>(ticks) << " tick(s)\"";
    process.addLog(oss.str());
}