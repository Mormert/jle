#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(int width, int height, const char* title);
	~Window();

	//Window(const Window &w) = delete;
	//Window& operator=(const Window &w) = delete;

	void SetResizeWindowEvent(void(*_event)(int, int));
	void SetKeyPressedEvent(void(*_event)(char));
	void SetKeyReleasedEvent(void(*_event)(char));

	void SetMainWindow();

	void PollEvents();
	void SwapBuffers();

	void FpsModeCursor(bool enabled);
	const bool& fpsModeEnabled;

	bool ShouldClose();

	GLFWwindow &GetNativeWindow();

	static Window* GetMainWindow();

private:
	GLFWwindow *glfwWindow;

	int window_width;
	int window_height;

	bool fpsModeEnabled_{ false };

	static void error_callback(int error, const char* description);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	static void(*resizeEvent)(int, int);
	static void(*keyPressedEvent)(char);
	static void(*keyReleasedEvent)(char);

	static Window* mainWindow;
};