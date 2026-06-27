#include "../process/Process.hpp"
#include "../scheduler/Scheduler.hpp"
#include "PrintCommand.hpp"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string PrintCommand::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "(%m/%d/%Y %I:%M:%S%p)");
    return oss.str();
}

void PrintCommand::execute(int coreId, Process& process) {
    std::ostringstream oss;
    oss << "(" << getTimestamp() << ") "
        << "Core:" << coreId;;

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