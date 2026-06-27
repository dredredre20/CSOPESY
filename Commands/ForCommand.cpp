#include "ForCommand.hpp"
#include "../process/Process.hpp"
#include <sstream>

void ForCommand::execute(int coreID, Process& process) {
    if (depth > MAX_DEPTH) return;

    for (int i = 0; i < iterations; ++i) {
        if (process.isSleeping()) break;

        for (auto& cmd : body) {
            if (process.isSleeping()) break;
            cmd->execute(coreID, process);
        }
    }
}