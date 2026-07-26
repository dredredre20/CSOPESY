cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)

# Add executable (list all your .cpp source files)
add_executable(MyProject
    main.cpp
    Process/Process.cpp
    Scheduler/FCFSScheduler.cpp
    Design_Utils/DesignAssets.cpp 
    Commands/PrintCommand.cpp
    Commands/DeclareCommand.cpp
    Commands/AddCommand.cpp
    Commands/SubtractCommand.cpp
    Commands/ForCommand.cpp
    Commands/SleepCommand.cpp
    SymbolTable/SymbolTable.cpp
    Scheduler/Scheduler.cpp 
    Scheduler/RRScheduler.cpp
    ConsoleUI/ConsoleManager.cpp
    ConsoleUI/AConsole.hpp
    ConsoleUI/MainMenuConsole.cpp
    ConsoleUI/ProcessConsole.cpp
    Memory/DemandPagingAllocator.cpp
    )

# Optional: include directories
# target_include_directories(MyProject PRIVATE include/)

# Optional: link libraries
# target_link_libraries(MyProject PRIVATE some_lib)