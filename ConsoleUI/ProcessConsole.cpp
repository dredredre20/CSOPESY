#include "ProcessConsole.hpp"
#include "ConsoleManager.hpp"
#include <iostream>

ProcessConsole::ProcessConsole(std::shared_ptr<Process> process, std::string process_name)
    : AConsole(PROCESS_CONSOLE){}

void ProcessConsole::onEnabled(){
    this->display();
}

void ProcessConsole::process(){
    if (this->refreshed == false){
        this -> refreshed = true;
        this -> printProcessInfo();
    }

    std::cout << "root:\\>";

    // Read user input
    std::string command;
    std::getline(std::cin, command);

    if (command == "process-smi"){
        this ->printProcessInfo();
    }   

    else if (command == "exit"){
        ConsoleManager::getInstance() -> returnToPreviousConsole();
        ConsoleManager::getInstance() -> unregisterScreen(this->name);
    }
}

void ProcessConsole::display(){
    // clear screen
    std::cout << "\033[2J\033[1;1H";
}


void ProcessConsole::printProcessInfo() const{
    //std::cout << process_name
}