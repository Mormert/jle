#include "Window.h"

#include "Input.h"

#include <iostream>

void(*Window::resizeEvent)(int, int) { nullptr };
void(*Window::keyPressedEvent)(char) { nullptr };
void(*Window::keyReleasedEvent)(char) { nullptr };

void Window::error_callback(int error, const char* description)
{
	std::cerr << "GLFW ERROR: " << description << '\n';
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	// Only used on key press, on key held down glfwGetKey is used instead
	if (keyPressedEvent && action == GLFW_PRESS) {
		(*keyPressedEvent)(key);
	}

	if (keyReleasedEvent && action == GLFW_RELEASE) {
		(*keyReleasedEvent)(key);
	}
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Input::SetScrollX(static_cast<float>(xoffset));
	Input::SetScrollY(static_cast<float>(yoffset));
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	if (resizeEvent) { (*resizeEvent)(width, height); }
}

Window* Window::mainWindow{ nullptr };

Window::Window(int width, int height, const char* title)
	: window_width{ width }, window_height{ height }, fpsModeEnabled{ fpsModeEnabled_ }
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		std::cerr << "GLFW ERROR: COULD NOT INITIALIZE";
		exit(1);
	}

	// Runs on OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// To Enable MSAA
	//glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (!glfwWindow)
	{
		glfwTerminate();
		std::cerr << "GLFW ERROR: COULD NOT CREATE WINDOW";
		exit(1);
	}

	glfwSetKeyCallback(glfwWindow, key_callback);
	glfwSetScrollCallback(glfwWindow, scroll_callback);
	glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);


	glfwMakeContextCurrent(glfwWindow);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "ERROR: Failed to initialize GLAD\n";
		exit(1);
	}

	glfwSwapInterval(0);

	int w, h;
	glfwGetFramebufferSize(glfwWindow, &w, &h);
	glViewport(0, 0, width, height);
}

Window::~Window()
{
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

void Window::SetResizeWindowEvent(void(*_event)(int, int))
{
	resizeEvent = _event;
}

void Window::SetKeyPressedEvent(void(*_event)(char))
{
	keyPressedEvent = _event;
}

void Window::SetKeyReleasedEvent(void(*_event)(char))
{
	keyReleasedEvent = _event;
}

void Window::SetMainWindow()
{
	mainWindow = this;
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(glfwWindow);
}

void Window::FpsModeCursor(bool enable)
{
	if (enable)
	{
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	fpsModeEnabled_ = enable;
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(glfwWindow);
}

GLFWwindow &Window::GetNativeWindow()
{
	return *glfwWindow;
}

Window* Window::GetMainWindow()
{
	return mainWindow;
}