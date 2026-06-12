#define STB_IMAGE_IMPLEMENTATION

#include <GLFW/glfw3.h>
#include "stb_image.h" // for processing images
#include <stdio.h>
#include "loadTexture.hpp"
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