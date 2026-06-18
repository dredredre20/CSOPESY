cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)

# Add executable (list all your .cpp source files)
add_executable(MyProject
    main.cpp
    process/process.cpp
    scheduler/FCFSScheduler.cpp
)

# Optional: include directories
# target_include_directories(MyProject PRIVATE include/)

# Optional: link libraries
# target_link_libraries(MyProject PRIVATE some_lib)