#include "MainMenuConsole.hpp"
#include "ConsoleManager.hpp"
#include "ProcessConsole.hpp"
#include "../Scheduler/FCFSScheduler.hpp"
#include "../Scheduler/RRScheduler.hpp"
#include <filesystem>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

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
        this -> handleScreenRCommand(command);
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

static bool runningProcesses = true; // flag for controlling
static void generateProcesses(Scheduler& scheduler, int numProcesses, const Config& cfg) {
    // further checking needed

    for (int i = 1; i <= numProcesses; ++i) {

        if (!runningProcesses) break;

        std::string name = "process" + std::string(i < 10 ? "0" : "") + std::to_string(i);
        std::shared_ptr<Process> p = std::make_shared<Process>(i, name);

        for (int j = 0; j < 100; ++j) {

            if (j % 2 == 0) {
                p->addCommand(ICommand::PRINT);
            }
            else if (j % 3 == 0) {
                p->addCommand(ICommand::SLEEP);
            }
            else if (j % 5 == 0) {
                p->addCommand(ICommand::DECLARE);
            }
            else if (j % 7 == 0) {
                p->addCommand(ICommand::ADD);
            }
            else if (j % 11 == 0) {
                p->addCommand(ICommand::SUBTRACT);
            }
            else {
                p->addCommand(ICommand::FOR);
            }
        }
        scheduler.addProcess(p);

        std::this_thread::sleep_for(std::chrono::milliseconds(cfg.batchFreq * 100));
    }
}

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
    processName.erase(std::remove(processName.begin(), processName.end(), '\''), processName.end());
    
    // Create the process
    static int nextProcessId = 1;
    auto newProcess = std::make_shared<Process>(nextProcessId++, processName);

    // Populate with dummy commands
    for (int i = 0; i < 100; ++i) {
        if (i % 2 == 0) newProcess->addCommand(ICommand::PRINT);
        else if (i % 3 == 0) newProcess->addCommand(ICommand::SLEEP);
        else if (i % 5 == 0) newProcess->addCommand(ICommand::DECLARE);
        else if (i % 7 == 0) newProcess->addCommand(ICommand::ADD);
        else newProcess->addCommand(ICommand::FOR);
    }

    // Add the process to the scheduler
    this->scheduler->addProcess(newProcess);

    auto pConsole = std::make_shared<ProcessConsole>(newProcess, processName);

    // Register and Switch
    auto manager = ConsoleManager::getInstance();
    manager->registerScreen(pConsole);
    manager->switchToScreen(processName);
}

// Logic for handling the screen -r command 
void MainMenuConsole::handleScreenRCommand(const std::string &input){
    std::string processName = input.substr(10);
    processName.erase(std::remove(processName.begin(), processName.end(), '\''), processName.end());

    std::cout << "[DEBUG] Attempting to attach to screen: '" << processName << "'..." << std::endl;
    
    auto manager = ConsoleManager::getInstance();

    // Check if the screen is already registered
    if (manager->hasScreen(processName)) {
        std::cout << "[DEBUG] Screen '" << processName << "' already exists in ConsoleManager. Switching..." << std::endl;
        manager->switchToScreen(processName);
    } 
    // Otherwise, look for it in the scheduler
    else {
        std::cout << "[DEBUG] Screen not in manager. Searching Scheduler for process '" << processName << "'..." << std::endl;
        
        std::shared_ptr<Process> target = scheduler->findProcessByName(processName);
        
        if (target != nullptr) {
            std::cout << "[DEBUG] Process found in scheduler! Creating new ProcessConsole." << std::endl;
            
            // Create the console on the fly
            auto pConsole = std::make_shared<ProcessConsole>(target, processName);
            manager->registerScreen(pConsole);
            
            std::cout << "[DEBUG] ProcessConsole registered. Switching to screen..." << std::endl;
            manager->switchToScreen(processName);
        } else {
            std::cout << "[DEBUG] Process '" << processName << "' not found in Ready, Running, or Finished queues." << std::endl;
            std::cout << "Error: Process '" << processName << "' not found.\n";
        }
    }
}


// Logic for handling the scheduler -start command
void MainMenuConsole::handleSchedulerStartCommand() {
    if (!this->scheduler) {
        std::cout << "Error: Scheduler not initialized.\n";
        return;
    }

    runningProcesses = true;
    scheduler->start();  // start threads first

    // Generate processes on a detached thread so CLI stays responsive
    std::thread([this]() {
        generateProcesses(*this->scheduler, 50, this->config);
        }).detach();

    std::cout << "Scheduler started.\n";
}

// Logic for handling the scheduler -stop command
void MainMenuConsole::handleSchedulerStopCommand() {
    if (!this->scheduler) {
        std::cout << "Error: Scheduler not initialized.\n";
        return;
    }
    runningProcesses = false;
    scheduler->stop();
    std::cout << "Scheduler stopped.\n";
}

// Logic for handling the report-util command
void MainMenuConsole::handleReportUtilCommand() {
    if (!this->scheduler) {
        std::cout << "Error: Scheduler not initialized.\n";
        return;
    }
    scheduler->reportUtil();
}

// Logic for handling the screen -ls command
void MainMenuConsole::handleScreenLsCommand() {
    if (!this->scheduler) {
        std::cout << "Error: Scheduler not initialized.\n";
        return;
    }
    scheduler->screenLs();
}

