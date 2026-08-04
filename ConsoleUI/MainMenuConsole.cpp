#include "MainMenuConsole.hpp"
#include "ConsoleManager.hpp"
#include "ProcessConsole.hpp"
#include "../Scheduler/FCFSScheduler.hpp"
#include "../Scheduler/RRScheduler.hpp"
#include "../Memory/MemoryUtils.hpp"
#include "../Memory/MemoryManager.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <random>
#include <cmath>
#include <algorithm>

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
static void generateProcesses(Scheduler& scheduler, const Config& cfg) {
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distIns(cfg.minIns, cfg.maxIns);

    auto validMemSizes = getPowersOfTwo(cfg.minMemPerProc, cfg.maxMemPerProc);
	std::uniform_int_distribution<> distMem(0, static_cast<int>(validMemSizes.size())  - 1);

    int processCounter = 1;
    size_t lastTickSeen = 0;
    
    while (runningProcesses) {
		size_t currentTicks = scheduler.activeCPUTicks + scheduler.idleCPUTicks;

        if (currentTicks - lastTickSeen < static_cast<size_t>(cfg.batchFreq)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        lastTickSeen = currentTicks;

        std::string name = "process" + std::string(processCounter < 10 ? "0" : "") + std::to_string(processCounter);

        int memSize = validMemSizes[distMem(gen)];
        auto memManager = MemoryManager::getInstance();

        std::shared_ptr<Process> p = std::make_shared<Process>(processCounter, name, memSize);
        p->generateInstructions(distIns(gen));

        scheduler.addProcess(p);
        processCounter++;
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

	// config.validateMemory(); // Validate memory constraints

    // 3. Initialize Memory Manager
    MemoryManager::getInstance()->initialize(
        static_cast<size_t>(config.maxOverallMem),
        static_cast<size_t>(config.memPerFrame)
    );

    // 4. Scheduler Setup
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
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distIns(config.minIns, config.maxIns);

    size_t memorySize = 0;

    try {
        memorySize = std::stoul(memorySizeStr);
    } catch (...) {
        std::cout << "Error: Invalid memory allocation.\n";
        return;
    }

	config.validateProcessMemSize(static_cast<int>(memorySize));

    // Create the process and assign memory 
    static int nextProcessId = 1;
    auto newProcess = std::make_shared<Process>(nextProcessId++, processName, memorySize);
    auto memManager = MemoryManager::getInstance();

	newProcess->setMemoryAllocator(memManager->getAllocator());
    
    // Populate with dummy commands
    newProcess->generateInstructions(distIns(gen));

    // Add the process to the scheduler
    this->scheduler->addProcess(newProcess);

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
    std::string instructions;

    if (!(iss >> processName)) {
        std::cout << "Error: Usage is 'screen -c <process_name> [process_memory_size] <instructions>'.\n";
        return;
    }

    size_t memorySize = config.minMemPerProc;  // Default to minimum allowed memory size

    // Skip the whitespace between the process name and whatever comes next
    while (iss.peek() == ' ') iss.get();
    if (iss.peek() != '"' && iss.peek() != std::char_traits<char>::eof()) {

        std::streampos beforeToken = iss.tellg();
        std::string maybeMemory;
        iss >> maybeMemory;
    }
    while (iss.peek() == ' ') iss.get();
    std::string rawInstructions;
    std::getline(iss, rawInstructions);

    // Strip exactly one pair of surrounding double quotes, if present
    if (rawInstructions.size() >= 2 && rawInstructions.front() == '"' && rawInstructions.back() == '"') {
        instructions = rawInstructions.substr(1, rawInstructions.size() - 2);
    } else {
        instructions = rawInstructions;
    }

    // Check if the command by the user follows proper size instructions
    size_t instructionCount = 1 + std::count(instructions.begin(), instructions.end(), ';');
    size_t is = instructions.find_first_not_of(" \t");

    if (instructions.empty() || instructionCount < 1 || instructionCount > 50 || is == std::string::npos) {
        std::cout << "Error: invalid command.\n";
        return;
    }

    // Create the process and assign memory
    static int nextProcessId = 1;
    auto newProcess = std::make_shared<Process>(nextProcessId++, processName, memorySize);
    auto memManager = MemoryManager::getInstance();

    // Validate if there is memory available
    if (memorySize > memManager->getFreeSize()) {
        std::cout << "Error: Not enough memory available for process '" << processName << "'.\n";
        return;
    }
    newProcess->setMemoryAllocator(memManager->getAllocator());

    // Parse the instructions provided by the user
    newProcess->parseInstructions(instructions);
    // Add the process to the scheduler
    this->scheduler->addProcess(newProcess);

 
    auto manager = ConsoleManager::getInstance();
    // Attach the process to the process console
    manager->attachProcess(newProcess);
    manager->switchConsole(PROCESS_CONSOLE);

}

// Logic for handling the scheduler -start command
void MainMenuConsole::handleSchedulerStartCommand() {
    // Generate processes on a detached thread so CLI stays responsive
    std::thread([this]() {
        generateProcesses(*this->scheduler, this->config);
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
    auto memManager = MemoryManager::getInstance();
    if (!memManager) {
        std::cout << "Error: Memory manager not initialized.\n";
        return;
    }
    scheduler->processSMI();
}


// Logic for handling the vmstat command
void MainMenuConsole::handleVmStatCommand(){
    auto memManager = MemoryManager::getInstance();
    if (!memManager) {
        std::cout << "Error: Memory manager not initialized.\n";
        return;
    }
    scheduler->vmstat();
}
