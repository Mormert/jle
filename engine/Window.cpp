#include "Window.h"

#include "3rdparty/glad/glad.h"
#include <GLFW/glfw3.h>

#include "Input.h"

#include <iostream>

namespace jle
{
	// Pimpl idiom implementation class
	class WindowImpl
	{
	public:

		static void error_callback(int error, const char* description)
		{
			std::cerr << "GLFW ERROR: " << description << '\n';
		}

		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
		{
			Input::SetScrollX(static_cast<float>(xoffset));
			Input::SetScrollY(static_cast<float>(yoffset));
		}

		static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);

			Window::GetMainWindow()->window_width = width;
			Window::GetMainWindow()->window_height = height;

			if (resizeEvent) { (*resizeEvent)(width, height); }
		}

		WindowImpl(int width, int height, const char* title)
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

			glfwSetWindowSizeLimits(glfwWindow, 240, 135, GLFW_DONT_CARE, GLFW_DONT_CARE);

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

		~WindowImpl()
		{
			glfwDestroyWindow(glfwWindow);
			glfwTerminate();
		}

		GLFWwindow* glfwWindow;

		static void(*resizeEvent)(int, int);
		static void(*keyPressedEvent)(char);
		static void(*keyReleasedEvent)(char);
	};

	void(*WindowImpl::resizeEvent)(int, int) { nullptr };
	void(*WindowImpl::keyPressedEvent)(char) { nullptr };
	void(*WindowImpl::keyReleasedEvent)(char) { nullptr };

	Window* Window::mainWindow{ nullptr };

	Window::Window(int width, int height, const char* title)
		: window_width{ width }, window_height{ height }, fpsModeEnabled{ fpsModeEnabled_ },
		pImpl{std::make_unique<WindowImpl>(width, height, title)} {}

	Window::~Window() = default;

	void Window::SetResizeWindowEvent(void(*event_)(int, int))
	{
		Pimpl()->resizeEvent = event_;
	}

	void Window::SetKeyPressedEvent(void(*event_)(char))
	{
		Pimpl()->keyPressedEvent = event_;
	}

	void Window::SetKeyReleasedEvent(void(*event_)(char))
	{
		Pimpl()->keyReleasedEvent = event_;
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
		glfwSwapBuffers(Pimpl()->glfwWindow);
	}

	void Window::FpsModeCursor(bool enable)
	{
		if (enable)
		{
			glfwSetInputMode(Pimpl()->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(Pimpl()->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		fpsModeEnabled_ = enable;
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(Pimpl()->glfwWindow);
	}

	Window* Window::GetMainWindow()
	{
		return mainWindow;
	}

	int Window::GetWindowHeight()
	{
		return window_height;
	}
		
	int Window::GetWindowWidth()
	{
		return window_width;
	}

	bool WindowGetKey(const Window& window, char key)
	{
		return glfwGetKey(window.Pimpl()->glfwWindow, key);
	}

	void WindowGetCursorPosition(const Window& window, double& x, double& y)
	{
		glfwGetCursorPos(window.Pimpl()->glfwWindow, &x, &y);
	}
}