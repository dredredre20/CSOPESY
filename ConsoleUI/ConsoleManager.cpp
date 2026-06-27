#include "ConsoleManager.hpp"
#include <iostream>


ConsoleManager* ConsoleManager::sharedInstance = nullptr;
ConsoleManager* ConsoleManager::getInstance(){
    return sharedInstance;
}

void ConsoleManager::initialize(){
    sharedInstance = new ConsoleManager();
}

void ConsoleManager::destroy(){
    delete sharedInstance;
}

void ConsoleManager::drawConsole() const{
    if (this->currentConsole != nullptr){
        this->currentConsole -> display();
    }

    else{
        std::cerr << "There is no assigned console. Please check." << std::endl;
    }
}

void ConsoleManager::process() const{
    if (this->currentConsole != nullptr){
        this->currentConsole->process();
    }
    else{
        std::cerr << "There is no assigned console. Please check." << std::endl;
    }
}

void ConsoleManager::switchConsole(std::string consoleName){
    if (this->consoleTable.find(consoleName) != consoleTable.end()){
        // clear screen
        std::cout << "\033[2J\033[1;1H";

        // for initializing the main menu at the start
        if (this -> previousConsole == nullptr){
            this -> currentConsole = this -> consoleTable[consoleName];
            this -> currentConsole -> onEnabled();
        }

        // for switching the console
        else{
            this -> previousConsole = this -> currentConsole;
            this -> currentConsole = this -> consoleTable[consoleName];
            this -> currentConsole -> onEnabled();
        }

    }

    else{
        std::cerr << "Console name " << consoleName << "not Found. Was it initialized?" << std::endl;
    }
}


// to be removed
void ConsoleManager::registerScreen(std::shared_ptr<AConsole> screenRef){
    if (this->consoleTable.find(screenRef->getName()) != consoleTable.end()){
        std::cerr << "Screen name " << screenRef->getName() << " already exists. Please use a different name." << std::endl;
        return; 
    }

    this->consoleTable[screenRef->getName()] = screenRef;
}

void ConsoleManager::attachProcess(std::shared_ptr<Process> process) {
    auto it = consoleTable.find(PROCESS_CONSOLE);
    if (it != consoleTable.end()) {
        auto processConsole = std::dynamic_pointer_cast<ProcessConsole>(it->second);
        if (processConsole) processConsole->setProcess(process);
    }
}

void ConsoleManager::detachProcess() {
    auto it = consoleTable.find(PROCESS_CONSOLE);
    if (it != consoleTable.end()) {
        auto processConsole = std::dynamic_pointer_cast<ProcessConsole>(it->second);
        if (processConsole) processConsole->setProcess(nullptr);
    }
}

ConsoleManager::ConsoleManager(){
    this->running = true;
    
    // initialize the consoles
    const std::shared_ptr<MainMenuConsole> mainMenuConsole = std::make_shared<MainMenuConsole>();
    
    // default process to null and process name as blank
    const std::shared_ptr<ProcessConsole> processConsole = std::make_shared<ProcessConsole>(nullptr, "");

    // store to console table
    this->consoleTable[MAIN_MENU_CONSOLE] = mainMenuConsole;
    this->consoleTable[PROCESS_CONSOLE] = processConsole;

    // first console is the main menu
    this->switchConsole(MAIN_MENU_CONSOLE);
}

bool ConsoleManager::hasScreen(std::string consoleName) const {
    return this->consoleTable.find(consoleName) != this->consoleTable.end();
}

void ConsoleManager::returnToPreviousConsole(){
    if (this->previousConsole != nullptr){
        std::cout << "\033[2J\033[1;1H";

        // Swap current and previous console
        std::shared_ptr<AConsole> temp = this->currentConsole;
        this->currentConsole = this->previousConsole;
        this->previousConsole = temp;

        this->currentConsole-> onEnabled();
    }

    else {
        std::cerr << "No previous console to return to." << std::endl;
    }
}

void ConsoleManager::exitApplication(){
    this->running = false;
}

bool ConsoleManager::isRunning() const{
    return this->running;
}