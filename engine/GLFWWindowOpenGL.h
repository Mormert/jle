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

	void SetWindowSettings(unsigned int width, unsigned int height, const std::string& title) override;

	void SetFpsMode(bool enable) override;
	bool IsFpsMode() override;

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

	// The window's width in pixels
	unsigned int window_width;
	// The window's height in pixels
	unsigned int window_height;

	float currentScrollX;
	float currentScrollY;

	std::string window_title;

	// Client tracking of if FPS mode is enabled or not
	bool fpsModeEnabled{ false };

private:
	std::shared_ptr<iRenderingInternalAPI> internalRenderingAPI;
};