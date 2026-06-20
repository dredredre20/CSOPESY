#include <iostream>
#include "DesignAssets.hpp"


 void designAssets::printASCIIart() {
     std::cout << "  ____ ____   ___  ____  _____ ______   __\n";
     std::cout << " / ___/ ___| / _ \\|    \\| ____/ ___\\ \\ / /\n";
     std::cout << "| |   \\___ \\| | | | |_) |  _| \\___ \\\\ V / \n";
     std::cout << "| |___ ___) | |_| |  __/| |___ ___) || |  \n";
     std::cout << " \\____|____/ \\___/|_|   |_____|____/ |_|  \n";
 }

 void designAssets::welcomeMenu() {
     printASCIIart();
     std::cout << "\nHello, Welcome to CSOPESY commandline!";
     std::cout << "\nType 'exit' to quit, 'clear' to clear screen";
     //std::cout << "\n\n** IMPORTANT: Type 'initialize' to load config and start system **\n";
 }


