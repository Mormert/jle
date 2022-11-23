// Copyright (c) 2022. Johan Lind

#include "initWindow.h"
#include "jleStaticOpenGLState.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include <GLFW/glfw3.h>

#include <iostream>

void *initWindow(int width, int height, const char *title) {

#ifdef BUILD_OPENGLES30
    // Runs on OpenGL ES 3.0
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
    // Runs on OpenGL Core 3.3
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // To Enable MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Avoid retina display scaling factor issues
    // TODO: Can setting this to false cause textures looking "off" and pixly?
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
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
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR: Failed to initialize GLAD\n";
        exit(1);
    }
#endif

    jleStaticOpenGLState::globalOpenGLInitialized = true;

    return glfwWindow;
}
