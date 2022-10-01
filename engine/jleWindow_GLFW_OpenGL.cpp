// Copyright (c) 2022. Johan Lind

#include "jleWindow_GLFW_OpenGL.h"
#include "jlePath.h"
#include "jlePathDefines.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#endif

#include <GLFW/glfw3.h>

#include <iostream>

#include "3rdparty/stb_image.h"

jleWindow_GLFW_OpenGL *jleWindow_GLFW_OpenGL::activeWindow{nullptr};

void jleWindow_GLFW_OpenGL::error_callback(int error, const char *description) {
    std::cerr << "GLFW ERROR: " << description << '\n';
}

void jleWindow_GLFW_OpenGL::key_callback(
    GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS) {
        sPressedKeys[key] = true;
    }

    if (action == GLFW_RELEASE) {
        sReleasedKeys[key] = true;
    }
}

void jleWindow_GLFW_OpenGL::scroll_callback(GLFWwindow *window,
                                            double xoffset,
                                            double yoffset) {
    activeWindow->currentScrollX = static_cast<float>(xoffset);
    activeWindow->currentScrollY = static_cast<float>(yoffset);
}

void jleWindow_GLFW_OpenGL::framebuffer_size_callback(GLFWwindow *window,
                                                      int width,
                                                      int height) {
    activeWindow->internalRenderingAPI->SetViewportDimensions(
        0,
        0,
        static_cast<unsigned int>(width),
        static_cast<unsigned int>(height));

    activeWindow->windowSettings.windowWidth = static_cast<unsigned int>(width);
    activeWindow->windowSettings.windowHeight =
        static_cast<unsigned int>(height);

    // Call all subscribed callbacks
    activeWindow->ExecuteResizeCallbacks(width, height);
}

#ifdef __EMSCRIPTEN__
extern "C" {
EMSCRIPTEN_KEEPALIVE
int resize_canvas_js(int width, int height) {

    glViewport(0, 0, width, height);

    printf("Change window size: %d, %d", width, height);

    const auto& window = jleWindow_GLFW_OpenGL::activeWindow;

    jleWindow_GLFW_OpenGL::framebuffer_size_callback(nullptr, width, height);

    return 1;
}
}
#endif

float jleWindow_GLFW_OpenGL::GetTime() {
    return static_cast<float>(glfwGetTime());
}

float jleWindow_GLFW_OpenGL::GetScrollX() {
    return activeWindow->currentScrollX;
}

float jleWindow_GLFW_OpenGL::GetScrollY() {
    return activeWindow->currentScrollY;
}

jleWindow_GLFW_OpenGL::~jleWindow_GLFW_OpenGL() {
    glfwDestroyWindow(nativeWindow);
    glfwTerminate();
}

void jleWindow_GLFW_OpenGL::SetWindowSettings(WindowSettings& windowSettings) {
    this->windowSettings = windowSettings;
}

void jleWindow_GLFW_OpenGL::DisplayCursor(bool enable) {
    if (enable) {
        glfwSetInputMode(nativeWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else {
        glfwSetInputMode(nativeWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    cursorVisible = enable;
}

bool jleWindow_GLFW_OpenGL::IsCursorDisplayed() { return cursorVisible; }

unsigned int jleWindow_GLFW_OpenGL::GetWindowHeight() {
    return windowSettings.windowHeight;
}

unsigned int jleWindow_GLFW_OpenGL::GetWindowWidth() {
    return windowSettings.windowWidth;
}

void jleWindow_GLFW_OpenGL::InitWindow(
    jleWindowInitializerInterface& windowInitializer,
    std::shared_ptr<jleRenderingInternalAPIInterface> internalRenderingAPI) {
    if (!internalRenderingAPI) {
        std::cerr << "Rendering API is null!\n";
        exit(1);
    }

    this->internalRenderingAPI = internalRenderingAPI;

    activeWindow = this;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cerr << "GLFW ERROR: COULD NOT INITIALIZE";
        exit(1);
    }

    if (windowSettings.windowIsRezisable) {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    }
    else {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }

    nativeWindow = static_cast<GLFWwindow *>(
        windowInitializer.InitWindow(windowSettings.windowWidth,
                                     windowSettings.windowHeight,
                                     windowSettings.WindowTitle.c_str()));

    glfwSetKeyCallback(nativeWindow, key_callback);
    glfwSetScrollCallback(nativeWindow, scroll_callback);
    glfwSetFramebufferSizeCallback(nativeWindow, framebuffer_size_callback);

    glfwSetWindowSizeLimits(nativeWindow,
                            windowSettings.windowWidthMin,
                            windowSettings.windowHeightMin,
                            GLFW_DONT_CARE,
                            GLFW_DONT_CARE);

    DisplayCursor(windowSettings.windowDisplayCursor);

#ifdef __EMSCRIPTEN__
    glfwSwapInterval(1);
#else
    if (windowSettings.windowCappedFps) {
        glfwSwapInterval(1);
    }
    else {
        glfwSwapInterval(0);
    }
#endif

    int w, h;
    glfwGetFramebufferSize(nativeWindow, &w, &h);
    internalRenderingAPI->SetViewportDimensions(
        0, 0, static_cast<unsigned int>(w), static_cast<unsigned int>(h));

    if (!windowSettings.iconPath.empty()) {
        GLFWimage images[1];
        images[0].pixels = stbi_load(jleAbsolutePath(windowSettings.iconPath)
                                         .GetAbsolutePathStr()
                                         .c_str(),
                                     &images[0].width,
                                     &images[0].height,
                                     nullptr,
                                     4);
#ifndef __linux__
        glfwSetWindowIcon(nativeWindow, 1, images);
#endif
        stbi_image_free(images[0].pixels);
    }
}

void jleWindow_GLFW_OpenGL::UpdateWindow() {
    activeWindow->currentScrollX = 0.f;
    activeWindow->currentScrollY = 0.f;

    std::memset(sPressedKeys, 0, sizeof(sPressedKeys));
    std::memset(sReleasedKeys, 0, sizeof(sPressedKeys));
    glfwPollEvents();
    glfwSwapBuffers(nativeWindow);
}

bool jleWindow_GLFW_OpenGL::WindowShouldClose() {
    return glfwWindowShouldClose(nativeWindow);
}

bool jleWindow_GLFW_OpenGL::GetKey(char key) {
    return glfwGetKey(nativeWindow, key);
}

bool jleWindow_GLFW_OpenGL::GetKeyPressed(char key) {
    return sPressedKeys[key];
}

bool jleWindow_GLFW_OpenGL::GetKeyReleased(char key) {
    return sReleasedKeys[key];
}

std::pair<int, int> jleWindow_GLFW_OpenGL::GetCursor() {
    double x, y;
    glfwGetCursorPos(nativeWindow, &x, &y);
    return std::pair<int, int>(static_cast<int>(x), static_cast<int>(y));
}

unsigned int jleWindow_GLFW_OpenGL::AddWindowResizeCallback(
    std::function<void(unsigned int, unsigned int)> callback) {
    unsigned int i = 0;

    // Find first available callback id
    for (auto it = windowResizedCallbacks.cbegin(),
              end = windowResizedCallbacks.cend();
         it != end && i == it->first;
         ++it, ++i) {
    }

    windowResizedCallbacks.insert(std::make_pair(
        i, std::bind(callback, std::placeholders::_1, std::placeholders::_2)));

    return i;
}

void jleWindow_GLFW_OpenGL::RemoveWindowResizeCallback(
    unsigned int callback_id) {
    windowResizedCallbacks.erase(callback_id);
}

void jleWindow_GLFW_OpenGL::ExecuteResizeCallbacks(int w, int h) {
    for (const auto& callback : activeWindow->windowResizedCallbacks) {
        callback.second(w, h);
    }
}

bool jleWindow_GLFW_OpenGL::GetMouseClick(int button) {
    return glfwGetMouseButton(nativeWindow, button);
}
