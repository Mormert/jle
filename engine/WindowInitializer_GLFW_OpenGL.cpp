// Copyright (c) 2022. Johan Lind

#include "WindowInitializer_GLFW_OpenGL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include <GLFW/glfw3.h>

#include <iostream>


InterfaceWindowPtr WindowInitializer_GLFW_OpenGL::InitWindow(int width, int height, const char *title) {

    // Runs on OpenGL ES 3.0
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // To Enable MSAA
    //glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!glfwWindow) {
        glfwTerminate();
        std::cerr << "GLFW ERROR: COULD NOT CREATE WINDOW";
        exit(1);
    }

    glfwMakeContextCurrent(glfwWindow);
#ifdef __EMSCRIPTEN__
#else
    if (!gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "ERROR: Failed to initialize GLAD\n";
        exit(1);
    }
#endif

    return glfwWindow;
}