#include "BaseScreen.hpp"
#include "ConsoleManager.hpp"

BaseScreen:: BaseScreen(std::shared_ptr<Process> process, std::string process_name) : AConsole(process_name) {
    this -> attachedProcess = process;
}

void BaseScreen::onEnabled(){

}

void BaseScreen::process(){
    if (this->refreshed == false){
        this -> refreshed = true;
        this -> printProcessInfo();
    }

    std::cout << "root:\\>";

    // Read user input
    std::string command;
    std::getline(std::cin, command);

    if (command == "process-smi"){
        this -> printProcessInfo();
    }

    else if (command == "exit"){
        ConsoleManager::getInstance() -> returnToPreviousConsole();
        ConsoleManager::getInstance() -> unregisterScreen(this->name);
        
    }
}

