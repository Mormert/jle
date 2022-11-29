// Copyright (c) 2022. Johan Lind

#include "jleWindow.h"
#include "initWindow.h"
#include "jleIncludeGL.h"
#include "jlePath.h"
#include "jlePathDefines.h"
#include "jleStaticOpenGLState.h"

#include <GLFW/glfw3.h>

#include <iostream>

#include "stb_image.h"

jleWindow *jleWindow::activeWindow{nullptr};

void
jleWindow::error_callback(int error, const char *description)
{
    LOGE << "GLFW ERROR: " << description << '\n';
}

void
jleWindow::glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

    if (action == GLFW_PRESS) {
        sPressedKeys[key] = true;
    }

    if (action == GLFW_RELEASE) {
        sReleasedKeys[key] = true;
    }
}

void
jleWindow::glfwScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    activeWindow->currentScrollX = static_cast<float>(xoffset);
    activeWindow->currentScrollY = static_cast<float>(yoffset);
}

void
jleWindow::glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    activeWindow->rendering->viewportDimensions(
        0, 0, static_cast<unsigned int>(width), static_cast<unsigned int>(height));

    activeWindow->windowSettings.width = static_cast<unsigned int>(width);
    activeWindow->windowSettings.height = static_cast<unsigned int>(height);

    // Call all subscribed callbacks
    activeWindow->executeResizeCallbacks(width, height);
}

#ifdef __EMSCRIPTEN__
extern "C" {
EMSCRIPTEN_KEEPALIVE
int
resize_canvas_js(int width, int height)
{

    glViewport(0, 0, width, height);

    printf("Change window size: %d, %d", width, height);

    const auto &window = jleWindow::activeWindow;

    jleWindow::glfwFramebufferSizeCallback(nullptr, width, height);

    return 1;
}
}
#endif

float
jleWindow::time()
{
    return static_cast<float>(glfwGetTime());
}

float
jleWindow::scrollX()
{
    return activeWindow->currentScrollX;
}

float
jleWindow::scrollY()
{
    return activeWindow->currentScrollY;
}

jleWindow::~jleWindow()
{
    glfwDestroyWindow(_glfwWindow);
    glfwTerminate();
}

void
jleWindow::settings(WindowSettings &windowSettings)
{
    this->windowSettings = windowSettings;
}

void
jleWindow::displayCursor(bool enable)
{
    if (enable) {
        glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    cursorVisible = enable;
}

bool
jleWindow::isCursorDisplayed() const
{
    return cursorVisible;
}

unsigned int
jleWindow::height() const
{
    return windowSettings.height;
}

unsigned int
jleWindow::width() const
{
    return windowSettings.width;
}

void
jleWindow::initWindow(std::shared_ptr<jleRendering> rendering)
{
    if (!rendering) {
        std::cerr << "Rendering API is null!\n";
        exit(1);
    }

    this->rendering = rendering;

    activeWindow = this;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cerr << "GLFW ERROR: COULD NOT INITIALIZE";
        exit(1);
    }

    if (windowSettings.isRezisable) {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }

    _glfwWindow = initWindow(windowSettings.width, windowSettings.height, windowSettings.WindowTitle.c_str());

    glfwSetKeyCallback(_glfwWindow, glfwKeyCallback);
    glfwSetScrollCallback(_glfwWindow, glfwScrollCallback);
    glfwSetFramebufferSizeCallback(_glfwWindow, glfwFramebufferSizeCallback);

    glfwSetWindowSizeLimits(
        _glfwWindow, windowSettings.widthMin, windowSettings.heightMin, GLFW_DONT_CARE, GLFW_DONT_CARE);

    displayCursor(windowSettings.shouldDisplayCursor);

#ifdef __EMSCRIPTEN__
    glfwSwapInterval(1);
#else
    if (windowSettings.isCappedFps) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
#endif

    int w, h;
    glfwGetFramebufferSize(_glfwWindow, &w, &h);
    rendering->viewportDimensions(0, 0, static_cast<unsigned int>(w), static_cast<unsigned int>(h));

    if (!windowSettings.iconPath.empty()) {
        GLFWimage images[1];
        images[0].pixels = stbi_load(jleAbsolutePath(windowSettings.iconPath).absolutePathStr().c_str(),
                                     &images[0].width,
                                     &images[0].height,
                                     nullptr,
                                     4);
#ifndef __linux__
        glfwSetWindowIcon(_glfwWindow, 1, images);
#endif
        stbi_image_free(images[0].pixels);
    }
}

void
jleWindow::updateWindow()
{
    activeWindow->currentScrollX = 0.f;
    activeWindow->currentScrollY = 0.f;

    std::memset(sPressedKeys, 0, sizeof(sPressedKeys));
    std::memset(sReleasedKeys, 0, sizeof(sPressedKeys));
    glfwPollEvents();
    glfwSwapBuffers(_glfwWindow);
}

bool
jleWindow::windowShouldClose()
{
    return glfwWindowShouldClose(_glfwWindow);
}

bool
jleWindow::key(char key)
{
    return glfwGetKey(_glfwWindow, key);
}

bool
jleWindow::keyPressed(char key)
{
    return sPressedKeys[key];
}

bool
jleWindow::keyReleased(char key)
{
    return sReleasedKeys[key];
}

std::pair<int, int>
jleWindow::cursor()
{
    double x, y;
    glfwGetCursorPos(_glfwWindow, &x, &y);
    return std::pair<int, int>(static_cast<int>(x), static_cast<int>(y));
}

unsigned int
jleWindow::addWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback)
{
    unsigned int i = 0;

    // Find first available callback id
    for (auto it = windowResizedCallbacks.cbegin(), end = windowResizedCallbacks.cend(); it != end && i == it->first;
         ++it, ++i) {
    }

    windowResizedCallbacks.insert(std::make_pair(i, std::bind(callback, std::placeholders::_1, std::placeholders::_2)));

    return i;
}

void
jleWindow::removeWindowResizeCallback(unsigned int callback_id)
{
    windowResizedCallbacks.erase(callback_id);
}

void
jleWindow::executeResizeCallbacks(int w, int h)
{
    for (const auto &callback : windowResizedCallbacks) {
        callback.second(w, h);
    }
}

bool
jleWindow::mouseClick(int button)
{
    return glfwGetMouseButton(_glfwWindow, button);
}
void
jleWindow::setCursorPosition(int x, int y)
{
    glfwSetCursorPos(_glfwWindow, x, y);
}
GLFWwindow *
jleWindow::initWindow(int width, int height, const char *title)
{
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
#ifndef __EMSCRIPTEN__
#ifdef BUILD_OPENGLES30
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR: Failed to initialize GLAD\n";
        exit(1);
    }
#else
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR: Failed to initialize GLAD\n";
        exit(1);
    }
#endif
#endif

    jleStaticOpenGLState::globalOpenGLInitialized = true;

    return glfwWindow;
}
