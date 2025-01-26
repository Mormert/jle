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

#include "jleWindowSettings.h"
#include "modules/jleGameModules.h"

struct GLFWwindow;

struct jleWindowDimensions {
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
    virtual ~jleWindow();

    static void error_callback(int error, const char *description);

    static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void glfwScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    static void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height);

    void settings(WindowSettings &windowSettings);

    void displayCursor(bool enable);

    bool isCursorDisplayed() const;

    void setCursorPosition(int x, int y);

    [[nodiscard]] unsigned int getHeight() const;

    [[nodiscard]] unsigned int getWidth() const;

    void initWindow();

    void updateWindow();

    bool windowShouldClose();

    bool getMouseClick(int button);

    [[nodiscard]] bool getKeyDown(int key) const;

    [[nodiscard]] bool getKeyPressed(int key) const;

    [[nodiscard]] bool getKeyReleased(int key) const;

    [[nodiscard]] float getScrollX() const;

    [[nodiscard]] float getScrollY() const;

    [[nodiscard]] std::pair<int, int> getCursor() const;

    [[nodiscard]] const jleWindowDimensions& getDimensions() const { return windowDimensions; }

    GLFWwindow* getGlfwWindow() const { return _glfwWindow; }

protected:
    virtual GLFWwindow *initGlfwWindow(int width, int height, const char *title);

    GLFWwindow *_glfwWindow = nullptr;

    WindowSettings windowSettings{};

    jleWindowDimensions windowDimensions{};

    float _currentScrollX{};
    float _currentScrollY{};

    bool _cursorVisible{false};

    // TODO: Use bitset instead of bool arrays for key states
    bool _pressedKeys[512] = {};
    bool _releasedKeys[512] = {};
};

class jleWindowModuleBase : public jleGameBaseModule
{
public:
    virtual void initWindowModule() {}
    virtual void updateWindowModule() {}

    virtual uint32_t getWindowWidth() = 0;
    virtual uint32_t getWindowHeight() = 0;

    virtual bool windowShouldClose() { return false; }
};

class jleGameWindowModule : public jleWindowModuleBase
{
public:
    void initWindowModule() override { window.initWindow(); }
    void updateWindowModule() override { window.updateWindow(); }

    uint32_t getWindowWidth() override { return window.getDimensions().framebufferWidth; }
    uint32_t getWindowHeight() override { return window.getDimensions().framebufferHeight; }

    bool windowShouldClose() override { return window.windowShouldClose(); }

    jleWindow& getWindow() { return window; }
    
private:
    jleWindow window;
};