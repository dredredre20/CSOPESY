#include "InstructionManager.hpp"
#include "../process/Process.hpp"
#include "../Commands/ICommand.hpp"
#include "../Design_Utils/DesignAssets.hpp"
#include "../Config.hpp"
#include "../Scheduler/FCFSScheduler.hpp"
#include "../Scheduler/RRScheduler.hpp"
#include "../Scheduler/Scheduler.hpp"
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
static std::unique_ptr<Scheduler> scheduler;
static Config config;
static bool runningProcesses = true; // flag for controlling 

// Generate processes with different command types
static void generateProcesses(Scheduler& scheduler, int numProcesses) {
    // further checking needed

	for (int i = 1; i <= numProcesses; ++i) {

        if (!runningProcesses) break;

		string name = "process" + string(i < 10 ? "0" : "") + to_string(i);
		Process p(i, name);
		
		for (int j = 0; j < 100; ++j) {

            if (j % 2 == 0){
                p.addCommand(ICommand::PRINT);
			}
			else if (j % 3 == 0) {
				p.addCommand(ICommand::SLEEP);
			}
			else if (j % 5 == 0) {
				p.addCommand(ICommand::DECLARE);
			}
			else if (j % 7 == 0) {
				p.addCommand(ICommand::ADD);
			}
			else if (j % 11 == 0) {
				p.addCommand(ICommand::SUBTRACT);
			}
			else {
				p.addCommand(ICommand::FOR);
			}
		}
		scheduler.addProcess(p);

        this_thread::sleep_for(chrono::milliseconds(500));
	}
}


bool InstructionManager::processCommand() {
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

		if (config.scheduler == "fcfs") {
			scheduler = std::make_unique<FCFSScheduler>();
		}

		else if (config.scheduler == "rr") { // pass time quantum
			scheduler = std::make_unique<RRScheduler>(config.quantumCycles);
		}

		else {
			cerr << "Error: Unknown scheduler type '" << config.scheduler << "' in config.txt." << endl;
			return true;
		}

        scheduler->initialize(config);
        initialized = true;
        cout << "Successfully initialized from: " << found.string() << "\n";

        return true;
    }

    if (!initialized) {
		if (command != "initialize" && command != "clear" && command != "exit") {
			cout << "Please run 'initialize' first.\n";
			return true;
		}
    }

    if (command == "scheduler-start") {
        scheduler->start();
        generateProcesses(*scheduler, 50); // 50 as base testing
        cout << "Scheduler started.\n"; 
    }
    
    else if (command == "scheduler-stop") {
        runningProcesses = false;
        scheduler->stop();
    }

    else if (command == "screen -ls") {
        scheduler->screenLs();
    }

    else if (command.rfind("screen -s ", 0) == 0) {
        // Extract process name from input
        std::string processName = command.substr(10);

        // Create a new process with the provided name
        static int nextProcessId = 1000; // Start custom process IDs at 1000

        // change this to find name in the ready queue for avoiding duplicates instead of random name
        Process newProcess(nextProcessId++, processName);

        for (int i = 0; i < 100; ++i) {
            if (i % 2 == 0) {
                newProcess.addCommand(ICommand::PRINT);
            }
            else if (i % 3 == 0) {
                newProcess.addCommand(ICommand::SLEEP);
            }
            else if (i % 5 == 0) {
                newProcess.addCommand(ICommand::DECLARE);
            }
            else if (i % 7 == 0) {
                newProcess.addCommand(ICommand::ADD);
            }
            else {
                newProcess.addCommand(ICommand::FOR);
            }
        }

        // Add the process to the scheduler
        scheduler->addProcess(newProcess);

        // now attach to the newly created process (same as screen -r)
        Process* target = scheduler->findProcessByName(processName);

        if (target == nullptr) {
            std::cout << "Failed to create or attach to process " << processName << ".\n";
        }
        else {
            system("cls");
            std::cout << "Successfully created and attached to process: " << processName << "\n\n";

            std::string screenCmd;
            while (true) {
                std::cout << "root:\\> ";
                std::getline(std::cin, screenCmd);

                if (screenCmd == "process-smi") {
                    std::cout << "Process name: " << target->getName() << "\n";
                    std::cout << "ID: " << target->getPID() << "\n";
                    std::cout << "Logs:\n";
                    // TODO: print logs here

                    if (target->isFinished()) {
                        std::cout << "Finished!\n";
                    }
                    else {
                        std::cout << "Current instruction line: " << target->getCommandCounter() << "\n";
                        std::cout << "Lines of code: " << target->getLinesOfCode() << "\n";
                    }
                }
                else if (screenCmd == "exit") {
                    system("cls");
                    break;
                }
                else {
                    std::cout << "Unknown command.\n";
                }
            }
        }
    }

    else if (command.rfind("screen -r ", 0) == 0) {

        // extract process name from input
        std::string processName = command.substr(10);
        Process* target = scheduler->findProcessByName(processName);

        if (target == nullptr) {
            std::cout << "Process " << processName << " not found.\n";
        }
        else {
            system("cls");
            std::string screenCmd;
            while (true) {
                std::cout << "root:\\> ";
                std::getline(std::cin, screenCmd);

                if (screenCmd == "process-smi") {
                    std::cout << "Process name: " << target->getName() << "\n";
                    std::cout << "ID: " << target->getPID() << "\n";
                    std::cout << "Logs:\n";
                    // TODO: print logs here

                    if (target->isFinished()) {
                        std::cout << "Finished!\n";
                    }
                    else {
                        std::cout << "Current instruction line: " << target->getCommandCounter() << "\n";
                        std::cout << "Lines of code: " << target->getLinesOfCode() << "\n";
                    }
                }
                else if (screenCmd == "exit") {
                    system("cls");
                    break;
                }
                else {
                    std::cout << "Unknown command.\n";
                }
            }
        }
    }

    // FOR CHECKING
    else if (command == "report-util") {
        scheduler->reportUtil();
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