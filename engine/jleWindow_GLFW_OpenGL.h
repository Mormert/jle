// Copyright (c) 2022. Johan Lind

#pragma once

#include "GLFW/glfw3.h"
#include "jleRendering.h"
#include "jleWindowSettings.h"

#include <map>
#include <memory>

class jleWindow_GLFW_OpenGL {
public:
    ~jleWindow_GLFW_OpenGL();

    static void error_callback(int error, const char *description);

    static void key_callback(
        GLFWwindow *window, int key, int scancode, int action, int mods);

    static void scroll_callback(GLFWwindow *window,
                                double xoffset,
                                double yoffset);

    static void framebuffer_size_callback(GLFWwindow *window,
                                          int width,
                                          int height);

    void SetWindowSettings(WindowSettings& windowSettings);

    void DisplayCursor(bool enable);

    bool IsCursorDisplayed();

    unsigned int GetWindowHeight();

    unsigned int GetWindowWidth();

    void InitWindow(std::shared_ptr<jleRendering> internalRenderingAPI);

    unsigned int AddWindowResizeCallback(
        std::function<void(unsigned int, unsigned int)> callback);

    void RemoveWindowResizeCallback(unsigned int callback_id);

    void ExecuteResizeCallbacks(int w, int h);

    void UpdateWindow();

    bool WindowShouldClose();

    float GetTime();

    bool GetMouseClick(int button);

    bool GetKey(char key);

    bool GetKeyPressed(char key);

    bool GetKeyReleased(char key);

    float GetScrollX();

    float GetScrollY();

    std::pair<int, int> GetCursor();

    static jleWindow_GLFW_OpenGL *activeWindow;

    GLFWwindow *GetGLFWWindow() { return nativeWindow; }

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

    std::shared_ptr<jleRendering> internalRenderingAPI;

    std::map<unsigned int, std::function<void(unsigned int, unsigned int)>>
        windowResizedCallbacks;
};
