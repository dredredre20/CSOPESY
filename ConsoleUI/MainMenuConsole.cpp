#include "MainMenuConsole.hpp"
#include "ConsoleManager.hpp"
#include "ProcessConsole.hpp"
#include "../Scheduler/FCFSScheduler.hpp"
#include "../Scheduler/RRScheduler.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

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

	else if (command.rfind("screen -c ", 0) == 0) {
		this->handleScreenCCommand(command);
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

    else if (command == "process-smi"){
        this -> handleProcessSMICommand();
    }

    else if (command == "vmstat"){
        this -> handleVmStatCommand();
    }

}



// Helper functions to process the commands

static bool runningProcesses = true; // flag for generating the dummy processes
static void generateProcesses(Scheduler& scheduler, int numProcesses, const Config& cfg) {
    // further checking neededc

    for (int i = 1; i <= numProcesses; ++i) {

        if (!runningProcesses) break;

        std::string name = "process" + std::string(i < 10 ? "0" : "") + std::to_string(i);
        std::shared_ptr<Process> p = std::make_shared<Process>(i, name);

        int numInstructions = cfg.minIns + (rand() % (cfg.maxIns - cfg.minIns + 1));

        p->generateInstructions(numInstructions);
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
        else if (key == "max-overall-mem") file >> config.maxOverallMem;
        else if (key == "mem-per-frame") file >> config.memPerFrame;
        else if (key == "min-mem-per-proc") file >> config.minMemPerProc;
        else if (key == "max-mem-per-proc") file >> config.maxMemPerProc;
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

    if (!this->scheduler) {
        std::cout << "Error: Scheduler not initialized.\n";
        return;
    }

    runningProcesses = true;
    scheduler->start();  // start the scheduler as soon as it is initialized
    std::cout << "Successfully initialized from: " << found.string() << std::endl;
}


// Logic for handling the exit command
void MainMenuConsole:: handleExitCommand(){
    if (!this->initialized){
        // If scheduler hasn't been initialized, dont stop the scheduler
        std::cout << "Exiting application..." << std::endl;
    }

    else{
        scheduler->stop(); // stop the scheduler
        std::cout << "Exiting application..." << std::endl;
    }
    ConsoleManager::getInstance()->exitApplication();
}

// Logic for handling the screen -s command 
void MainMenuConsole:: handleScreenSCommand(const std::string &input){
    // Ensure the scheduler exists
    if (!this->scheduler) {
        std::cout << "Error: Scheduler not initialized. Run 'initialize' first.\n";
        return;
    }

    // Extract screen -s parameters
    std::string args = input.substr(10);
    args.erase(std::remove(args.begin(), args.end(), '\''), args.end());

    // Split into process name and memory size
    std::istringstream iss(args);
    std::string processName;
    std::string memorySizeStr;
    iss >> processName >> memorySizeStr;

    // Error handling when parameter is invalid
    if (processName.empty() || memorySizeStr.empty()){
        std::cout << "Error: Usage is 'screen -s <process_name> <process_memory_size>'.\n";
        return;
    }

    // Validate memory size (should be a power of 2, in rage [2^6, 2^16])
    size_t memorySize = 0;

    try {
        memorySize = std::stoul(memorySizeStr);
    } catch (...) {
        std::cout << "Error: Invalid memory allocation.\n";
        return;
    }

    bool isPowerOfTwo = (memorySize > 0) && ((memorySize & (memorySize - 1)) == 0);
    bool inRange = (memorySize >= 64 && memorySize <= 65536); // 2^6 to 2^16

    if (!isPowerOfTwo || !inRange) {
        std::cout << "invalid memory allocation\n";
        return;
    }

    // Create the process and assign memory 
    static int nextProcessId = 1;
    auto newProcess = std::make_shared<Process>(nextProcessId++, processName);
    int numInstructions = config.minIns + (rand() % (config.maxIns - config.minIns + 1));

    // Populate with dummy commands
    newProcess->generateInstructions(numInstructions);

    // Add the process to the scheduler
    this->scheduler->addProcess(newProcess);
    
    // Wait until scheduler assigns a core
    while (newProcess->getCPUCoreID() == -1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    auto manager = ConsoleManager::getInstance();

    // Attach the process to the process console
    manager->attachProcess(newProcess);

    manager->switchConsole(PROCESS_CONSOLE);

}

// Logic for handling the screen -r command 
void MainMenuConsole::handleScreenRCommand(const std::string &input){
    std::string processName = input.substr(10);
    processName.erase(std::remove(processName.begin(), processName.end(), '\''), processName.end());

    // Check if the process exists
    std::shared_ptr<Process> process = scheduler->findProcessByName(processName);

    // Check if process is already terminated due to memory access violation
    if (process != nullptr && process->getState() == Process::TERMINATED_VIOLATION) {
        std::cout << "\nProcess " << process->getName()
            << " shut down due to memory access violation error that occurred at "
            << process->getViolationTimestamp() << ". 0x"
            << std::hex << process->getViolationAddress() << " invalid."
            << std::dec << std::endl;

        return; // do not switch consoles anymore
    }

    // If process has not been terminated yet due to memory access violation
    if (process != nullptr){
        auto manager = ConsoleManager::getInstance();
        // Attach the process to the process console
        manager->attachProcess(process);

        // Switch console
        manager->switchConsole(PROCESS_CONSOLE);
    }

    // Otherwise
    else {
        std::cerr << "Error: Process '" << processName << "' not found.\n";
    }
}

void MainMenuConsole::handleScreenCCommand(const std::string& input) {
    std::string args = input.substr(10);
    args.erase(std::remove(args.begin(), args.end(), '\''), args.end());

    std::istringstream iss(args);
    std::string processName;
    std::string memorySizeStr;
    std::string instructions;

	if (!(iss >> processName >> memorySizeStr)) {
		std::cout << "Error: Usage is 'screen -c <process_name> <process_memory_size> <instructions>'.\n";
		return;
	}

    size_t memorySize = 0;

    try {
        memorySize = std::stoul(memorySizeStr);
    }
    catch (...) {
        std::cout << "Error: Invalid memory allocation.\n";
        return;
    }

    std::getline(iss, instructions);
    std::shared_ptr<Process> process = scheduler->findProcessByName(processName);
    process->parseInstructions(instructions);
}

// Logic for handling the scheduler -start command
void MainMenuConsole::handleSchedulerStartCommand() {
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


// Logic for handling the process-smi command
void MainMenuConsole::handleProcessSMICommand() {

    // Memory can be accessed through the scheduler, so call the SMI through there

    if (!this->scheduler) {
        std::cout << "Error: Scheduler not initialized.\n";
    } else {
        this->scheduler->visualizeHighLevelMemory();
    }

    std::cout << std::endl;
    
}


// Logic for handling the vmstat command
void MainMenuConsole::handleVmStatCommand(){

    if (!this->scheduler) {
        std::cout << "Error: Scheduler not initialized.\n";
    } else {
        this->scheduler->visualizeDetailedMemory();
    }

    std::cout << std::endl;

}
