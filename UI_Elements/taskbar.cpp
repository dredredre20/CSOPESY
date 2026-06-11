#define IMGUI_DEFINE_MATH_OPERATORS
#define STB_IMAGE_IMPLEMENTATION

#include <GLFW/glfw3.h>
#include <cstdio>
#include <ctime>
#include <string>
#include <chrono>
#include  "taskbar.hpp"
#include "imgui.h"
#include "UIConfig.hpp"
#include "stb_image.h" // for processing images
#include <stdio.h>

// Helper function to load an image file and returns an OpenGL Texture ID
GLuint loadTexture(const char* filename) {
    int width, height, channels;
    // Load image data from disk
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4); 
    if (data == nullptr) {
        printf("Hello");
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload the raw pixel data into the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Free the CPU memory since it's safely copied to the GPU now
    stbi_image_free(data);

    return textureID;
}

void Taskbar::initialize() {
    GLuint filesIcon = loadTexture("assets/folder.png");

    taskbarIcons = {
        { "Files",  filesIcon, ImVec4(1,1,1,1), []() { /* open file browser */ } },
        { "INIT",   0, ImVec4(0.4f, 0.8f, 1.0f, 1), []() { /* init action */ } },
        { "START",  0, ImVec4(0.4f, 0.9f, 0.4f, 1), []() { /* start action */ } },
        { "STOP",   0, ImVec4(0.9f, 0.4f, 0.4f, 1), []() { /* stop action */ } },
        { "SRCH",   0, ImVec4(0.7f, 0.4f, 0.9f, 1), []() { /* search action */ } },
    };
}

void Taskbar::draw() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float taskbarHeight = 60.0f * UIConfig::getScaleFactor();

    // Position at bottom of screen
    ImGui::SetNextWindowPos(ImVec2(0, displaySize.y - taskbarHeight));
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, taskbarHeight));

    ImGui::Begin("Taskbar", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar);

    // Render icons
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));
    // initialize();
    for (auto& icon : taskbarIcons) {
        // If the icon has an image (texture), execute
        if (icon.texture) {
            if (ImGui::ImageButton(
                icon.name.c_str(),                          // unique string ID
                (ImTextureID)(intptr_t)icon.texture,        // texture ID
                ImVec2(40, 40) * UIConfig::getScaleFactor() // size
            )) {
                icon.onClick();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", icon.name.c_str());
            }
        } 
        // Otherwise, 
        else {
            if (ImGui::Button(icon.name.c_str(), ImVec2(60, 40) * UIConfig::getScaleFactor())) {
                icon.onClick();
            }
        }
        ImGui::SameLine();
    }
    

    ImGui::PopStyleVar();

    ImGui::End();
}
