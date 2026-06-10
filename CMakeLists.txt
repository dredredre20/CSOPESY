cmake_minimum_required(VERSION 3.20)
project(csopesy_os CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/imgui")
set(GLFW_ROOT "${CMAKE_SOURCE_DIR}/lib/glfw")

set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/imgui_impl_opengl3.cpp"
)

add_executable(${PROJECT_NAME}
    main.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${IMGUI_DIR}"
    "${GLFW_ROOT}/include"
)

if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    "${GLFW_ROOT}/glfw3.lib"
    opengl32.lib
    gdi32.lib
    imm32.lib
    dwmapi.lib
)

# Comment this out during development to allow the console to be visible for debugging purposes
#set_target_properties(${PROJECT_NAME} PROPERTIES
    #WIN32_EXECUTABLE TRUE
#)