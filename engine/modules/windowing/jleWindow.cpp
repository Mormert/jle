/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleWindow.h"
#include "jleProfiler.h"

#if JLE_BUILD_RUNTIME_CONFIGURABLE
#include "core/jleCommandArguments.h"
#endif

#include <iostream>

#include "stb_image.h"
#include <plog/Log.h>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windef.h>
#include <windowsx.h>
#endif

// JLE_EXTERN_TEMPLATE_CEREAL_CPP(WindowSettings)

void
jleWindow::error_callback(int error, const char *description)
{
    LOGE << "GLFW ERROR: " << description << '\n';
}

void
jleWindow::glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto thiz = reinterpret_cast<jleWindow *>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
        thiz->_pressedKeys[key] = true;
    }

    if (action == GLFW_RELEASE) {
        thiz->_releasedKeys[key] = true;
    }
}

void
jleWindow::glfwScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    auto thiz = reinterpret_cast<jleWindow *>(glfwGetWindowUserPointer(window));

    thiz->_currentScrollX = static_cast<float>(xoffset);
    thiz->_currentScrollY = static_cast<float>(yoffset);
}

void
jleWindow::glfwFramebufferSizeCallback(GLFWwindow *window, int fbWidth, int fbHeight)
{
    auto thiz = reinterpret_cast<jleWindow *>(glfwGetWindowUserPointer(window));

    thiz->windowSettings.width = static_cast<unsigned int>(fbWidth);
    thiz->windowSettings.height = static_cast<unsigned int>(fbHeight);

    jleWindowResizeEvent resizeEvent{};

    auto monitor = glfwGetWindowMonitor(window);
    if (!monitor) {
        monitor = glfwGetPrimaryMonitor();
    }

    // Get the physical size of the monitor in millimeters
    glfwGetMonitorPhysicalSize(monitor, &resizeEvent.monitorPhysicalSizeWidth, &resizeEvent.monitorPhysicalSizeHeight);

    glfwGetMonitorContentScale(monitor, &resizeEvent.contentScaleX, &resizeEvent.contentScaleY);

    // Calculate DPI
    float dpiX = (float)fbWidth / ((float)resizeEvent.monitorPhysicalSizeWidth / 25.4f);
    float dpiY = (float)fbHeight / ((float)resizeEvent.monitorPhysicalSizeHeight / 25.4f);

    resizeEvent.framebufferWidth = fbWidth;
    resizeEvent.framebufferHeight = fbHeight;
    resizeEvent.dpiWidth = dpiX;
    resizeEvent.dpiHeight = dpiY;

    // Call all subscribed callbacks
    thiz->executeResizeCallbacks(resizeEvent);
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
jleWindow::scrollX() const
{
    return _currentScrollX;
}

float
jleWindow::scrollY() const
{
    return _currentScrollY;
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
    _cursorVisible = enable;
}

bool
jleWindow::isCursorDisplayed() const
{
    return _cursorVisible;
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

#if defined(WIN32) && JLE_BUILD_EDITOR

WNDPROC g_glfwWndProc = nullptr;

LRESULT CALLBACK
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    constexpr int customTitleBarHeight = 30;
    static bool resizing = false;
    static POINT lastCursorPosition;

    RECT windowRect{};

    switch (uMsg) {
    case WM_NCCALCSIZE:
        // Remove standard window frame
        return 0;
    case WM_NCHITTEST:
        POINT cursor;
        GetCursorPos(&cursor);
        ScreenToClient(hWnd, &cursor);

        if (ImGui::IsAnyItemHovered()) {
            return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam);
        }

        GetClientRect(hWnd, &windowRect);

        // Adjust the bottom border for resizing
        windowRect.bottom += 5;

        if (cursor.y >= windowRect.bottom - 5) {
            if (cursor.x < 5)
                return HTBOTTOMLEFT;
            else if (cursor.x > windowRect.right - 5)
                return HTBOTTOMRIGHT;
            else
                return HTBOTTOM;
        } else if (cursor.x < 5) {
            if (cursor.y < 5)
                return HTTOPLEFT;
            else
                return HTLEFT;
        } else if (cursor.x > windowRect.right - 5) {
            if (cursor.y < 5)
                return HTTOPRIGHT;
            else
                return HTRIGHT;
        } else if (cursor.y < customTitleBarHeight) {
            if (cursor.y < 5) {
                if (cursor.x < windowRect.right / 2)
                    return HTTOPLEFT;
                else
                    return HTTOPRIGHT;
            } else
                return HTCAPTION;
        }
        return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam); // Return default handling for other areas
    case WM_LBUTTONDOWN:
        if (wParam == HTTOPLEFT || wParam == HTTOPRIGHT || wParam == HTBOTTOMLEFT || wParam == HTBOTTOMRIGHT ||
            wParam == HTBOTTOM || wParam == HTLEFT || wParam == HTRIGHT) {
            resizing = true;
            lastCursorPosition = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            SetCapture(hWnd);
            return 0;
        }
        return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam);
    case WM_MOUSEMOVE:
        if (resizing) {
            POINT currentCursorPosition = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int deltaX = currentCursorPosition.x - lastCursorPosition.x;
            int deltaY = currentCursorPosition.y - lastCursorPosition.y;

            if (wParam == HTTOPLEFT || wParam == HTLEFT || wParam == HTBOTTOMLEFT) {
                windowRect.left += deltaX;
                windowRect.top += deltaY;
            }
            if (wParam == HTTOPLEFT || wParam == HTTOP || wParam == HTTOPRIGHT) {
                windowRect.top += deltaY;
            }
            if (wParam == HTTOPLEFT || wParam == HTTOPRIGHT || wParam == HTBOTTOMRIGHT) {
                windowRect.right += deltaX;
            }
            if (wParam == HTTOPRIGHT || wParam == HTRIGHT || wParam == HTBOTTOMRIGHT) {
                windowRect.bottom += deltaY;
            }

            SetWindowPos(hWnd,
                         NULL,
                         windowRect.left,
                         windowRect.top,
                         windowRect.right - windowRect.left,
                         windowRect.bottom - windowRect.top,
                         SWP_NOZORDER | SWP_NOREDRAW);
            lastCursorPosition = currentCursorPosition;
            return 0;
        }
        return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam);
    case WM_LBUTTONUP:
        if (resizing) {
            resizing = false;
            ReleaseCapture();
            return 0;
        }
        return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam);
    case WM_GETMINMAXINFO: {
        // Set maximum and minimum window size and position based on screen dimensions
        auto *pmmi = (MINMAXINFO *)lParam;

        pmmi->ptMaxPosition.x = 0;
        pmmi->ptMaxPosition.y = 0;

        // Get the monitor where the window is located
        HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

        if (hMonitor) {
            MONITORINFO monitorInfo;
            monitorInfo.cbSize = sizeof(MONITORINFO);
            if (GetMonitorInfo(hMonitor, &monitorInfo)) {
                RECT workArea = monitorInfo.rcWork;
                int workAreaWidth = workArea.right - workArea.left;
                int workAreaHeight = workArea.bottom - workArea.top;

                pmmi->ptMaxSize.x = workAreaWidth;
                pmmi->ptMaxSize.y = workAreaHeight;
            }
        }

        return 0;
    }
    default:
        return CallWindowProc(g_glfwWndProc, hWnd, uMsg, wParam, lParam);
    }
}

GLFWwindow *
initWindowForEditor_Win32()
{
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    RECT workArea;
    int workAreaWidth = mode->width;
    int workAreaHeight = mode->height;

    if (SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0)) {
        workAreaWidth = ((float)workArea.right - (float)workArea.left);
        workAreaHeight = ((float)workArea.bottom - (float)workArea.top);
    }

    auto glfwWindow = jleWindow::initGlfwWindow(workAreaWidth, workAreaHeight, "");

    // Get the native window handle
    HWND hwnd = glfwGetWin32Window(glfwWindow);

    // Saves a reference to the WndProc function set from GLFW (found in glfw's win32_window.c)
    g_glfwWndProc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);

    // Set the window procedure callback
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);

    // WE_EX_TOOLWINDOW is used to combat an issue where the window borders would be "outside the screen"
    // WS_EX_APPWINDOW is to combat an issue which is caused by WE_EX_TOOLWINDOW, which is that it disables the
    // program from showing up in the Windows taskbar
    SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_APPWINDOW);

    // Maximises the window on startup
    ShowWindow(hwnd, SW_MAXIMIZE);

    return glfwWindow;
}

#endif // WIN32 && JLE_BUILD_EDITOR

void
jleWindow::initWindow()
{
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

    JLE_EXEC_IF(JLE_BUILD_EDITOR)
    {
#if defined(WIN32) && JLE_BUILD_EDITOR
        _glfwWindow = initWindowForEditor_Win32();
#else
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        _glfwWindow = initGlfwWindow(mode->width, mode->height, windowSettings.WindowTitle.c_str());

#ifdef __APPLE__
        customizeTitleBarMacOS(_glfwWindow);
#endif

#endif
    }
    else
    {
#if JLE_BUILD_RUNTIME_CONFIGURABLE
        std::string windowTitle =
            windowSettings.WindowTitle + " [" + jleCommandArguments::getInstance().getProgramArgsString() + "]";
#else
        std::string windowTitle = windowSettings.WindowTitle;
#endif
        _glfwWindow = initGlfwWindow(windowSettings.width, windowSettings.height, windowTitle.c_str());
    }

    glfwSetWindowUserPointer(_glfwWindow, this);

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
    JLE_SCOPE_PROFILE_CPU(WindowUpdate);

    _currentScrollX = 0.f;
    _currentScrollY = 0.f;

    std::memset(_pressedKeys, 0, sizeof(_pressedKeys));
    std::memset(_releasedKeys, 0, sizeof(_pressedKeys));
    glfwPollEvents();
    glfwSwapBuffers(_glfwWindow);
}

bool
jleWindow::windowShouldClose()
{
    JLE_SCOPE_PROFILE_CPU(WindowCloseCheck);
    return glfwWindowShouldClose(_glfwWindow);
}

bool
jleWindow::keyDown(int key) const
{
    return glfwGetKey(_glfwWindow, key);
}

bool
jleWindow::keyPressed(int key) const
{
    return _pressedKeys[key];
}

bool
jleWindow::keyReleased(int key) const
{
    return _releasedKeys[key];
}

std::pair<int, int>
jleWindow::cursor() const
{
    double x, y;
    glfwGetCursorPos(_glfwWindow, &x, &y);
    return std::pair<int, int>(static_cast<int>(x), static_cast<int>(y));
}

unsigned int
jleWindow::addWindowResizeCallback(std::function<void(const jleWindowResizeEvent &resizeEvent)> callback)
{
    unsigned int i = 0;

    // Find first available callback id
    for (auto it = windowResizedCallbacks.cbegin(), end = windowResizedCallbacks.cend(); it != end && i == it->first;
         ++it, ++i) {
    }

    windowResizedCallbacks.insert(
        std::make_pair(i, [callback](auto &&PH1) { callback(std::forward<decltype(PH1)>(PH1)); }));

    return i;
}

void
jleWindow::removeWindowResizeCallback(unsigned int callback_id)
{
    windowResizedCallbacks.erase(callback_id);
}

void
jleWindow::executeResizeCallbacks(const jleWindowResizeEvent &resizeEvent)
{
    for (const auto &callback : windowResizedCallbacks) {
        callback.second(resizeEvent);
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
    // Note: This does not work on OS X (tested, and info online confirms this)
    LOGE << "OpenGL Debug Output (id=" << id << "): " << message;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        LOGE << "Source: API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        LOGE << "Source: Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        LOGE << "Source: Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        LOGE << "Source: Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        LOGE << "Source: Application";
        break;
    default:
    case GL_DEBUG_SOURCE_OTHER:
        LOGE << "Source: Other";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        LOGE << "Type: Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        LOGE << "Type: Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        LOGE << "Type: Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        LOGE << "Type: Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        LOGE << "Type: Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        LOGE << "Type: Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        LOGE << "Type: Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        LOGE << "Type: Pop Group";
        break;
    default:
    case GL_DEBUG_TYPE_OTHER:
        LOGE << "Type: Other";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        LOGE << "Severity: high";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        LOGE << "Severity: medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        LOGE << "Severity: low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        LOGE << "Severity: notification";
        break;
    }
}
#endif
#endif

GLFWwindow *
jleWindow::initGlfwWindow(int width, int height, const char *title)
{
    JLE_EXEC_IF(JLE_BUILD_OPENGLES30)
    {
        // Runs on OpenGL ES 3.0
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    }
    else
    {
        // Runs on OpenGL Core 3.3
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    }
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

    // Special case for Mac's Retina screens
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
#endif

    GLFWwindow *glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!glfwWindow) {
        glfwTerminate();
        std::cerr << "GLFW ERROR: COULD NOT CREATE WINDOW";
        exit(1);
    }

    glfwMakeContextCurrent(glfwWindow);
#ifndef __EMSCRIPTEN__
    JLE_EXEC_IF(JLE_BUILD_OPENGLES30)
    {
        if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "ERROR: Failed to initialize GLAD\n";
            exit(1);
        }
    }
    else
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "ERROR: Failed to initialize GLAD\n";
            exit(1);
        }
    }
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

    return glfwWindow;
}

GLFWwindow *
jleWindow::glfwWindow()
{
    return _glfwWindow;
}
