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
 "UI_Elements/desktop.cpp" "UI_Elements/taskbar.cpp" "UI_Elements/UIConfig.cpp")

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GL_SILENCE_DEPRECATION)
endif()

# For Mac
if(APPLE)
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${IMGUI_DIR}"
        "/opt/homebrew/include"
    )
# For Windows
else()
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${IMGUI_DIR}"
        "${GLFW_ROOT}/include"
    )
endif()

if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
    )
endif()

# For mac
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "/opt/homebrew/lib/libglfw.3.dylib"
    )
# For windows
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "${GLFW_ROOT}/glfw3.lib"
        opengl32.lib
        gdi32.lib
        imm32.lib
        dwmapi.lib
    )
endif()

if(MSVC)
    add_compile_options(/ZI)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)

# Comment this out during development to allow the console to be visible for debugging purposes
#set_target_properties(${PROJECT_NAME} PROPERTIES
    #WIN32_EXECUTABLE TRUE
#)