#include <GLFW/glfw3.h>
#include "imgui.h"
#include "UIConfig.hpp"

void UIConfig::initialize() {
    // Get primary monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // Calculate scale factor based on resolution
    float baseWidth = 1920.0f;
    float currentWidth = static_cast<float>(mode->width);
    scaleFactor = currentWidth / baseWidth;

        // Clamp to reasonable range
    if (scaleFactor < 1.0f) scaleFactor = 1.0f;
    if (scaleFactor > 2.0f) scaleFactor = 2.0f;
}

float UIConfig::getScaleFactor() {
    return scaleFactor;
}

ImVec2 UIConfig::scale(ImVec2 size) {
   return ImVec2(size.x * scaleFactor, size.y * scaleFactor);
}

static float scaleFactor;


float UIConfig::scaleFactor = 1.0f;