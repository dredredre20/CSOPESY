// a template for the process
#pragma once
#include <iostream>
#include <string>
#include <cstdlib> // for rand() function
#include <fstream>
#include <chrono>

#include "process.h"

    // Execute one instruction of the process
   void Process::executeInstruction(std::ofstream& outFile) {

        if (remainingInstructions > 0) {
            outFile << "Executing instruction for Process " << id << ": " << name << std::endl;
            // std::cout << "Executing instruction for Process " << id << ": " << name << std::endl;
            remainingInstructions--;
        }
        else {
            outFile << "Process " << id << ": " << name << " has already finished.\n";
            // std::cout << "Process " << id << ": " << name << " has already finished.\n";
        }
    }

    // Get the remaining number of instructions
    int Process::getRemainingInstructions() const {
        return remainingInstructions;
    }

    int Process::getID() const {
        return id;
    }

    // Check if the process has finished
    bool Process::hasFinished() const {
        return remainingInstructions == 0;
    }




/* int main() {
    // Create a sample process
    Process myProcess("SampleProcess", 1, 10);

    // Execute instructions until the process finishes
    while (!myProcess.hasFinished()) {
        myProcess.executeInstruction();
    }

    std::cout << "Process has finished!\n";

    return 0;
} */