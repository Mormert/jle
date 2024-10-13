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

#pragma once

#include "core/jleCommon.h"

#include "jleWindowSettings.h"

#include <map>
#include <memory>

struct GLFWwindow;

#ifdef __APPLE__
void customizeTitleBarMacOS(GLFWwindow *window);
#endif

struct jleWindowResizeEvent {
    // Main framebuffer dimensions
    int framebufferWidth;
    int framebufferHeight;

    // The physical size of the monitor in millimeters
    int monitorPhysicalSizeWidth;
    int monitorPhysicalSizeHeight;

    float dpiWidth;
    float dpiHeight;

    // The content scale is the ratio between the current DPI and the platform's default DPI
    float contentScaleX;
    float contentScaleY;
};

class jleWindow
{
public:
    ~jleWindow();

    static void error_callback(int error, const char *description);

    static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void glfwScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    static void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height);

    void settings(WindowSettings &windowSettings);

    void displayCursor(bool enable);

    bool isCursorDisplayed() const;

    void setCursorPosition(int x, int y);

    [[nodiscard]] unsigned int height() const;

    [[nodiscard]] unsigned int width() const;

    void initWindow();

    unsigned int addWindowResizeCallback(std::function<void(const jleWindowResizeEvent &resizeEvent)> callback);

    void removeWindowResizeCallback(unsigned int callback_id);

    void executeResizeCallbacks(const jleWindowResizeEvent &resizeEvent);

    void updateWindow();

    bool windowShouldClose();

    float time();

    bool mouseClick(int button);

    bool keyDown(int key) const;

    bool keyPressed(int key) const;

    bool keyReleased(int key) const;

    float scrollX() const;

    float scrollY() const;

    std::pair<int, int> cursor() const;

    GLFWwindow *glfwWindow();

    static GLFWwindow *initGlfwWindow(int width, int height, const char *title);

private:
    GLFWwindow *_glfwWindow;

    WindowSettings windowSettings;

    float _currentScrollX;
    float _currentScrollY;

    bool _cursorVisible{false};

    bool _pressedKeys[512];
    bool _releasedKeys[512];

    std::map<unsigned int, std::function<void(const jleWindowResizeEvent &resizeEvent)>> windowResizedCallbacks;
};
