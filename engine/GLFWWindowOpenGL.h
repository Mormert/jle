#pragma once

#include "iWindowInternalAPI.h"

#include "GLFW/glfw3.h"

#include <memory>

class GLFWWindowOpenGL : public iWindowInternalAPI
{
public:

	~GLFWWindowOpenGL();

	static void error_callback(int error, const char* description);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	void SetWindowSettings(WindowSettings& windowSettings) override;

	void DisplayCursor(bool enable) override;
	bool IsCursorDisplayed() override;

	unsigned int GetWindowHeight() override;
	unsigned int GetWindowWidth() override;

	void InitWindow(iWindowInitializer& windowInitializer, std::shared_ptr<iRenderingInternalAPI> internalRenderingAPI) override;

	void UpdateWindow() override;

	bool WindowShouldClose() override;

	bool GetKey(char key) override;
	float GetScrollX() override;
	float GetScrollY() override;
	std::pair<int, int> GetCursor() override;

	static GLFWWindowOpenGL* activeWindow;

protected:
	// Native handle to GLFW window
	GLFWwindow* nativeWindow;

	WindowSettings windowSettings;

	float currentScrollX;
	float currentScrollY;

	bool cursorVisible{ false };

private:
	std::shared_ptr<iRenderingInternalAPI> internalRenderingAPI;
};