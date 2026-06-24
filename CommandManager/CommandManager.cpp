#include "CommandManager.hpp"
#include "../process/Process.hpp"
#include "../Commands/ICommand.hpp"
#include "../Design_Utils/DesignAssets.hpp"
#include "../Config.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <optional>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

static bool initialized = false;
static Config config;

bool CommandManager::processCommand() {
    designAssets da;

    string command;
    cout << "\nEnter a command: ";
    getline(std::cin, command);

    if (command == "initialize") { // Read config.txt file
        // Robust search for config.txt: try working directory, executable dir, and several parents.
        std::vector<std::filesystem::path> candidates;
        candidates.emplace_back(std::filesystem::current_path() / "config.txt");

#ifdef _WIN32
        // Try executable directory on Windows
        char exePathBuf[MAX_PATH];
        DWORD len = GetModuleFileNameA(NULL, exePathBuf, MAX_PATH);
        if (len > 0 && len < MAX_PATH) {
            std::filesystem::path exePath(exePathBuf);
            candidates.emplace_back(exePath.parent_path() / "config.txt");
        }
#endif

        // Walk up a few parent directories from current path
        std::filesystem::path p = std::filesystem::current_path();
        for (int i = 0; i < 5 && !p.empty(); ++i) {
            candidates.emplace_back(p / "config.txt");
            if (p.has_parent_path()) p = p.parent_path();
            else break;
        }

        // Try source-file based path
        try {
            std::filesystem::path srcPath(__FILE__);
            auto repoRoot = srcPath.parent_path().parent_path();
            candidates.emplace_back(repoRoot / "config.txt");
        } catch (...) {
            // ignore
        }

        std::filesystem::path found;
        for (const auto &c : candidates) {
            if (std::filesystem::exists(c)) {
                found = c;
                break;
            }
        }
        if (found.empty()) {
            cerr << "Error: Could not find config.txt. Tried multiple locations. Current working directory: "
                 << std::filesystem::current_path() << endl;
            return true;
        }

        std::ifstream file(found.string());
        if (!file.is_open()) {
            cerr << "Error: Found config.txt at " << found.string() << " but failed to open it." << endl;
            return true;
        }

        string key;
        while (file >> key) {
            if (key == "num-cpu") {
                file >> config.numCPU;
            }
            else if (key == "scheduler") {
                file >> config.scheduler;
                // Strip surrounding quotes if present
                if (config.scheduler.size() >= 2 && config.scheduler.front() == '"' && config.scheduler.back() == '"') {
                    config.scheduler = config.scheduler.substr(1, config.scheduler.size() - 2);
                }
            }
            else if (key == "quantum-cycles") {
                file >> config.quantumCycles;
            }
            else if (key == "batch-process-freq") {
                file >> config.batchFreq;
            }
            else if (key == "min-ins") {
                file >> config.minIns;
            }
            else if (key == "max-ins") {
                file >> config.maxIns;
            }
            else if (key == "delay-per-exec") {
                file >> config.delayPerExec;
            }
            else {
                // Skip unknown value token to avoid infinite loop if file malformed
                string skip;
                file >> skip;
            }
        }

        file.close();
        initialized = true;
        cout << "Successfully initialized from: " << found.string() << "\n";

        // TODO: Pass config instance to Scheduler;
        return true;
    }

    if (!initialized) {
		if (command != "initialize" || command != "clear" || command != "exit") {
			cout << "Please run 'initialize' first.\n";
			return true;
		}
    }

	// TODO: Create Scheduler class to return the correct scheduler based on config.scheduler
    if (command == "scheduler-start") {
        // Run the main scheduler loop on a separate background thread
        schedulerThread = thread(&FCFSScheduler::runScheduler, &fcfs);
        schedulerThread.detach();
        cout << "Scheduler started.\n"; 
    }
    
    else if (command == "scheduler-stop") {
        fcfs.stopScheduler();
    }

    // TODO: Add commands screen -s -> process-smi, screen -r
    else if (command == "screen -ls") {
        fcfs.screenLs();
    }

    // TODO: Implementation for report-util
    else if (command == "report-util") {
        std::cout << command << " command recognized. Doing something.";
    }

    else if (command == "clear") {
        std::cout << "\033[2J\033[1;1H"; // Clears screen and moves cursor to top-left
        da.welcomeMenu();
    }

    else if (command == "exit") {
        return false;
    }

    else {
        std::cout << "Unknown command. Try again";
    }

    return true;
}