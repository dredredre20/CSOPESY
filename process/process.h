// a template for the process
#pragma once
#include <iostream>
#include <string>
#include <cstdlib> // for rand() function
#include <fstream>
#include <chrono>

class Process {
private:
    std::string name;
    int id;
    int totalInstructions;
    int remainingInstructions;

public:
    Process(const std::string& processName, int processId, int numInstructions)
        : name(processName), id(processId), totalInstructions(numInstructions), remainingInstructions(numInstructions) {}

    // Execute one instruction of the process
    void executeInstruction(std::ofstream& outFile);

    // Get the remaining number of instructions
    int getRemainingInstructions() const;

    int getID() const;

    std::string getName() const;

    // Check if the process has finished
    bool hasFinished() const;
};
