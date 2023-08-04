// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleWindowSettings.h"
#include <map>
#include <memory>

class GLFWwindow;

class jleWindow {
public:
    ~jleWindow();

    static void error_callback(int error, const char *description);

    static void glfwKeyCallback(
        GLFWwindow *window, int key, int scancode, int action, int mods);

    static void glfwScrollCallback(GLFWwindow *window,
                                   double xoffset,
                                   double yoffset);

    static void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height);

    void settings(WindowSettings &windowSettings);

    void displayCursor(bool enable);

    bool isCursorDisplayed() const;

    void setCursorPosition(int x, int y);

    [[nodiscard]] unsigned int height() const;

    [[nodiscard]] unsigned int width() const;

    void initWindow();

    unsigned int addWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback);

    void removeWindowResizeCallback(unsigned int callback_id);

    void executeResizeCallbacks(int w, int h);

    void updateWindow();

    bool windowShouldClose();

    float time();

    bool mouseClick(int button);

    bool key(int key);

    bool keyPressed(int key);

    bool keyReleased(int key);

    float scrollX();

    float scrollY();

    std::pair<int, int> cursor();

    static jleWindow *activeWindow;

    GLFWwindow *
    glfwWindow()
    {
        return _glfwWindow;
    }

protected:
    GLFWwindow *_glfwWindow;

    WindowSettings windowSettings;

    float currentScrollX;
    float currentScrollY;

    bool cursorVisible{false};

private:
    static GLFWwindow *initGlfwWindow(int width, int height, const char *title);
    inline static bool sPressedKeys[512];
    inline static bool sReleasedKeys[512];

    std::map<unsigned int, std::function<void(unsigned int, unsigned int)>>
        windowResizedCallbacks;
};
