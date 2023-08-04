// Copyright (c) 2023. Johan Lind

#include "jleWindow.h"
#include "jleStaticOpenGLState.h"

#include <iostream>

#include "stb_image.h"
#include <plog/Log.h>

JLE_EXTERN_TEMPLATE_CEREAL_CPP(WindowSettings)


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
    _activeWindow->currentScrollX = static_cast<float>(xoffset);
    _activeWindow->currentScrollY = static_cast<float>(yoffset);
}

void
jleWindow::glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height)
{

    _activeWindow->windowSettings.width = static_cast<unsigned int>(width);
    _activeWindow->windowSettings.height = static_cast<unsigned int>(height);

    // Call all subscribed callbacks
    _activeWindow->executeResizeCallbacks(width, height);
}

#ifdef __EMSCRIPTEN__
extern "C" {
EMSCRIPTEN_KEEPALIVE
int
resize_canvas_js(int width, int height)
{

    glViewport(0, 0, width, height);

    printf("Change window size: %d, %d", width, height);

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
    return _activeWindow->currentScrollX;
}

float
jleWindow::scrollY()
{
    return _activeWindow->currentScrollY;
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
jleWindow::initWindow()
{
    _activeWindow = this;
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

#ifdef BUILD_EDITOR
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    _glfwWindow = initGlfwWindow(mode->width, mode->height, windowSettings.WindowTitle.c_str());
#else
    _glfwWindow = initGlfwWindow(windowSettings.width, windowSettings.height, windowSettings.WindowTitle.c_str());
#endif

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

    if (!windowSettings.iconPath.isEmpty()) {
        GLFWimage images[1];
        images[0].pixels =
            stbi_load(windowSettings.iconPath.getRealPath().c_str(), &images[0].width, &images[0].height, nullptr, 4);
#ifndef __linux__
        glfwSetWindowIcon(_glfwWindow, 1, images);
#endif
        stbi_image_free(images[0].pixels);
    }
}

void
jleWindow::updateWindow()
{
    _activeWindow->currentScrollX = 0.f;
    _activeWindow->currentScrollY = 0.f;

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
jleWindow::key(int key)
{
    return glfwGetKey(_glfwWindow, key);
}

bool
jleWindow::keyPressed(int key)
{
    return sPressedKeys[key];
}

bool
jleWindow::keyReleased(int key)
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

#ifndef __EMSCRIPTEN__
#ifndef NDEBUG
void APIENTRY
glDebugOutput(GLenum source,
              GLenum type,
              unsigned int id,
              GLenum severity,
              GLsizei length,
              const char *message,
              const void *userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    // ADD BREAKPOINT ON SOME LINE BELOW TO FIND THE CALLING OPENGL FUNCTION THAT CAUSED THE ERROR!
    // Note: This does not work on OS X (tested, and info online says this)
    std::cout << "OpenGL Debug Output (id=" << id << "): " << message << std::endl;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        std::cout << "Source: API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cout << "Source: Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cout << "Source: Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cout << "Source: Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cout << "Source: Application";
        break;
    default:
    case GL_DEBUG_SOURCE_OTHER:
        std::cout << "Source: Other";
        break;
    }
    std::cout << std::endl;

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "Type: Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "Type: Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "Type: Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "Type: Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "Type: Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cout << "Type: Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cout << "Type: Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cout << "Type: Pop Group";
        break;
    default:
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "Type: Other";
        break;
    }
    std::cout << std::endl;

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "Severity: high";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "Severity: medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "Severity: low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cout << "Severity: notification";
        break;
    }
}
#endif
#endif

GLFWwindow *
jleWindow::initGlfwWindow(int width, int height, const char *title)
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

#ifndef __EMSCRIPTEN__
#ifndef NDEBUG
    // Debugging with immediate error messages require the system to run OpenGL 4.3 +
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
#endif

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

    // Set the debug output for OpenGL errors
#ifndef __EMSCRIPTEN__
#ifndef NDEBUG
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
#endif
#endif

    jleStaticOpenGLState::globalOpenGLInitialized = true;

    return glfwWindow;
}

GLFWwindow *
jleWindow::glfwWindow()
{
    return _glfwWindow;
}
