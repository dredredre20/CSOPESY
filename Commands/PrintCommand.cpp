#include "../Process/Process.hpp"
#include "../Scheduler/Scheduler.hpp"
#include "PrintCommand.hpp"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

void PrintCommand::execute(int coreId, Process& process) {
    std::ostringstream oss;
    
    // If msg contains a variable
    if (varName.has_value()) {
        if (!process.hasVariable(varName.value()))
            process.setVariable(varName.value(), 0);

        uint16_t val = static_cast<uint16_t>(process.getVariable(varName.value()));
        oss << toPrint << val;
    }
    else {
        oss << toPrint;
    }

    process.addLog(oss.str());
}