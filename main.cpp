#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include "UI_Elements/desktop.hpp"
#include "UI_Elements/taskbar.hpp"
#include "UI_Elements/UIManager.hpp"
#include "UI_Elements/Windows.hpp"

int main()
{
    if (!glfwInit())
    {
        printf("[ERROR] glfwInit() failed\n");
        return -1;
    }

    // --- CROSS-PLATFORM OPENGL SETUP ---
    #ifdef __APPLE__
        // macOS strictly requires OpenGL 3.3 Core Profile + Forward Compatibility
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #else
        // Windows/Linux setup
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif
    // ------------------------------------

    GLFWwindow* window = glfwCreateWindow(960, 540, "CSOPESY Desktop OS Emulator", nullptr, nullptr);
    if (!window)
    {
        printf("[ERROR] glfwCreateWindow() failed\n");
        glfwTerminate();
        return -1;
    }
    printf("[OK] GLFW window created\n");
    glfwMakeContextCurrent(window);

    //gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    printf("[OK] Dear ImGui initialized\n");

    Desktop desktop;
    Taskbar taskbar;
    desktop.loadWallpaper("assets/trees.png"); // feel free to change this
    taskbar.initialize(window);

    // Register windows with the UIManager
    UIManager::getInstance().registerWindow("File Browser", std::make_shared<FileBrowserWindow>());
    UIManager::getInstance().registerWindow("Initialize", std::make_shared<InitWindow>());
    UIManager::getInstance().registerWindow("Task Manager", std::make_shared<TaskManagerWindow>());


    // Kernel initialization
    // Start system services

    printf("[OK] Entering main loop\n");
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // draw the underlying OS canvas layer
        desktop.renderDesktop(window);
        
        // draw the taskbar
        taskbar.draw();
        
        // render active layers
        UIManager::getInstance().updateAllWindows();
        UIManager::getInstance().renderAllWindows();

        ImGui::Render(); 
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}