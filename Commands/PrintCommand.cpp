#include "PrintCommand.hpp"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

PrintCommand::PrintCommand(int p_id, std::string processName, std::string toPrint)
    : ICommand(p_id, PRINT) {
    this->processName = processName;
    this->toPrint = toPrint;
}

std::string PrintCommand::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "(%m/%d/%Y %I:%M:%S%p)");
    return oss.str();
}

void PrintCommand::execute(int coreId) {

    std::string filename = this->processName + ".txt";

    // Check if file is empty/new to write the header once
    bool isNewFile = false;
    {
        std::ifstream checkFile(filename);
        isNewFile = !checkFile.good() || checkFile.peek() == std::ifstream::traits_type::eof();
    }

    std::ofstream logFile(filename, std::ios::app);
    if (!logFile.is_open()) {
        return;
    }

    if (isNewFile) {
        logFile << "Logs:\n";
    }

    logFile << getTimestamp()
            << " Core:" << coreId
            << " \"" << this->toPrint << "\"\n";

    logFile.close();
}