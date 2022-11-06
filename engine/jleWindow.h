// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleRendering.h"
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

    void initWindow(std::shared_ptr<jleRendering> internalRenderingAPI);

    unsigned int addWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback);

    void removeWindowResizeCallback(unsigned int callback_id);

    void executeResizeCallbacks(int w, int h);

    void updateWindow();

    bool windowShouldClose();

    float time();

    bool mouseClick(int button);

    bool key(char key);

    bool keyPressed(char key);

    bool keyReleased(char key);

    float scrollX();

    float scrollY();

    std::pair<int, int> cursor();

    static jleWindow *activeWindow;

    GLFWwindow *glfwWindow() { return nativeWindow; }

protected:
    // Native handle to GLFW window
    GLFWwindow *nativeWindow;

    WindowSettings windowSettings;

    float currentScrollX;
    float currentScrollY;

    bool cursorVisible{false};

private:
    inline static bool sPressedKeys[256];
    inline static bool sReleasedKeys[256];

    std::shared_ptr<jleRendering> rendering;

    std::map<unsigned int, std::function<void(unsigned int, unsigned int)>>
        windowResizedCallbacks;
};
