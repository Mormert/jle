// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleWindowInternalAPIInterface.h"

#include "GLFW/glfw3.h"

#include <map>
#include <memory>

class jleWindow_GLFW_OpenGL : public jleWindowInternalAPIInterface {
public:
    ~jleWindow_GLFW_OpenGL() override;

    static void error_callback(int error, const char *description);

    static void key_callback(
        GLFWwindow *window, int key, int scancode, int action, int mods);

    static void scroll_callback(GLFWwindow *window,
                                double xoffset,
                                double yoffset);

    static void framebuffer_size_callback(GLFWwindow *window,
                                          int width,
                                          int height);

    void SetWindowSettings(WindowSettings& windowSettings) override;

    void DisplayCursor(bool enable) override;

    bool IsCursorDisplayed() override;

    unsigned int GetWindowHeight() override;

    unsigned int GetWindowWidth() override;

    void InitWindow(
        jleWindowInitializerInterface& windowInitializer,
        std::shared_ptr<jleRenderingAPI_OpenGL> internalRenderingAPI) override;

    unsigned int AddWindowResizeCallback(
        std::function<void(unsigned int, unsigned int)> callback) override;

    void RemoveWindowResizeCallback(unsigned int callback_id) override;

    void ExecuteResizeCallbacks(int w, int h) override;

    void UpdateWindow() override;

    bool WindowShouldClose() override;

    float GetTime() override;

    bool GetMouseClick(int button) override;

    bool GetKey(char key) override;

    bool GetKeyPressed(char key) override;

    bool GetKeyReleased(char key) override;

    float GetScrollX() override;

    float GetScrollY() override;

    std::pair<int, int> GetCursor() override;

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

    std::shared_ptr<jleRenderingAPI_OpenGL> internalRenderingAPI;

    std::map<unsigned int, std::function<void(unsigned int, unsigned int)>>
        windowResizedCallbacks;
};
