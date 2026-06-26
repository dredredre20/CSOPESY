#include "MainMenuConsole.hpp"
#include "ConsoleManager.hpp"
#include "ProcessConsole.hpp"
#include "../Scheduler/FCFSScheduler.hpp"
#include "../Scheduler/RRScheduler.hpp"
#include <filesystem>
#include <fstream>

MainMenuConsole::MainMenuConsole() : AConsole(MAIN_MENU_CONSOLE){}

void MainMenuConsole::onEnabled(){
    this -> display();
}

void MainMenuConsole::display(){
    this -> printMenu();
}

void MainMenuConsole::printMenu() const{
    std::cout << "  ____ ____   ___  ____  _____ ______   __\n";
    std::cout << " / ___/ ___| / _ \\|    \\| ____/ ___\\ \\ / /\n";
    std::cout << "| |   \\___ \\| | | | |_) |  _| \\___ \\\\ V / \n";
    std::cout << "| |___ ___) | |_| |  __/| |___ ___) || |  \n";
    std::cout << " \\____|____/ \\___/|_|   |_____|____/ |_|  \n";
    std::cout << "----------------------------------------------\n";
    std::cout << "Hello, Welcome to CSOPESY Emulator!" << std::endl;

    std::cout << "\nDevelopers: " << std::endl;
    std::cout << "Chu, Andre Benedict" << std::endl;
    std::cout << "Monloy, Kharlene" << std::endl;
    std::cout << "Rocha, Angelo" << std::endl;
    std::cout << "Tolentino, Nathaniel" << std::endl;
}

// Funtion to recognize the command input 
void MainMenuConsole::process(){
    std::cout << "\nroot:\\>";
    std::string command;
    getline(std::cin, command);

    // ---  Command Recognition ---

    // Check first if initialized
    if (!this->initialized) {
        if (command != "initialize" && command != "exit") {
            std::cout << "Please run 'initialize' first.\n";
            return;
        }
    }

    if (command == "initialize"){
        this -> handleInitializeCommand();
    }

    else if (command == "exit"){
        this -> handleExitCommand();
    }

    else if (command.rfind("screen -s ", 0) == 0){
        this -> handleScreenSCommand(command);
    }

    else if (command.rfind("screen -r ", 0) == 0) {
        this -> handleScreenRCommand();
    }

    else if (command == "scheduler-start"){
        this -> handleSchedulerStartCommand();
    }

    else if (command == "scheduler-stop"){
        this -> handleSchedulerStopCommand();
    }

    else if (command == "report-util"){
        this -> handleReportUtilCommand();
    }

    else if (command == "screen -ls"){
        this -> handleScreenLsCommand();
    }


}



// --------- Command Interpreter Implementation ---------
// (Includes helper functions to process the commands)


// Logic for handling the initialize command
void MainMenuConsole::handleInitializeCommand() {

    // Path Finding Logic
    std::vector<std::filesystem::path> candidates;
    candidates.emplace_back(std::filesystem::current_path() / "config.txt");

    #ifdef _WIN32
        char exePathBuf[MAX_PATH];
        DWORD len = GetModuleFileNameA(NULL, exePathBuf, MAX_PATH);
        if (len > 0 && len < MAX_PATH) {
            std::filesystem::path exePath(exePathBuf);
            candidates.emplace_back(exePath.parent_path() / "config.txt");
        }
    #endif

    std::filesystem::path p = std::filesystem::current_path();
    for (int i = 0; i < 5 && !p.empty(); ++i) {
        candidates.emplace_back(p / "config.txt");
        if (p.has_parent_path()) p = p.parent_path();
        else break;
    }

    std::filesystem::path found;
    for (const auto &c : candidates) {
        if (std::filesystem::exists(c)) {
            found = c;
            break;
        }
    }

    if (found.empty()) {
        std::cerr << "Error: Could not find config.txt." << std::endl;
        return;
    }

    // 2. File Parsing Logic
    std::ifstream file(found.string());
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open config.txt." << std::endl;
        return;
    }

    std::string key;
    while (file >> key) {
        if (key == "num-cpu") file >> config.numCPU;
        else if (key == "scheduler") {
            file >> config.scheduler;
            if (config.scheduler.size() >= 2 && config.scheduler.front() == '"' && config.scheduler.back() == '"')
                config.scheduler = config.scheduler.substr(1, config.scheduler.size() - 2);
        }
        else if (key == "quantum-cycles") file >> config.quantumCycles;
        else if (key == "batch-process-freq") file >> config.batchFreq;
        else if (key == "min-ins") file >> config.minIns;
        else if (key == "max-ins") file >> config.maxIns;
        else if (key == "delay-per-exec") file >> config.delayPerExec;
        else {
            std::string skip; file >> skip;
        }
    }
    file.close();

    // 3. Scheduler Setup
    if (config.scheduler == "fcfs") {
        this->scheduler = std::make_unique<FCFSScheduler>();
    }
    else if (config.scheduler == "rr") {
        this->scheduler = std::make_unique<RRScheduler>(config.quantumCycles);
    }
    else {
        std::cerr << "Error: Unknown scheduler type '" << config.scheduler << "'." << std::endl;
        return;
    }

    this->scheduler->initialize(config);
    this->initialized = true;
    std::cout << "Successfully initialized from: " << found.string() << std::endl;
}


// Logic for handling the exit command
void MainMenuConsole:: handleExitCommand(){
    std::cout << "Exiting application..." << std::endl;
    ConsoleManager::getInstance()->exitApplication();
}

// Logic for handling the screen -s command 
void MainMenuConsole:: handleScreenSCommand(const std::string &input){
    // Ensure the scheduler exists
    if (!this->scheduler) {
        std::cout << "Error: Scheduler not initialized. Run 'initialize' first.\n";
        return;
    }

    // Extract the process name
    std::string processName = input.substr(10);

    // Create the process
    static int nextProcessId = 1;
    Process newProcess(nextProcessId++, name);

    // Populate with dummy commands
    for (int i = 0; i < 100; ++i) {
        if (i % 2 == 0) newProcess.addCommand(ICommand::PRINT);
        else if (i % 3 == 0) newProcess.addCommand(ICommand::SLEEP);
        else if (i % 5 == 0) newProcess.addCommand(ICommand::DECLARE);
        else if (i % 7 == 0) newProcess.addCommand(ICommand::ADD);
        else newProcess.addCommand(ICommand::FOR);
    }

    this->scheduler->addProcess(newProcess);

    std::cout << "Successfully created process: " << name << " (PID: " << (nextProcessId - 1) << ")\n";

}

// Logic for handling the screen -r command 
void MainMenuConsole:: handleScreenRCommand(){
    
}


// Logic for handling the scheduler -start command
void MainMenuConsole:: handleSchedulerStartCommand(){
    
}

// Logic for handling the scheduler -stop command
void MainMenuConsole:: handleSchedulerStopCommand(){
    
}

// Logic for handling the report-util command
void MainMenuConsole:: handleReportUtilCommand(){
    
}

// Logic for handling the screen -ls command
void MainMenuConsole:: handleScreenLsCommand(){
    
}
