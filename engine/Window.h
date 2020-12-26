#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace jle
{
	
	class Window final
	{
	public:

		~Window();

		Window(const Window &w) = delete;
		Window& operator=(const Window &w) = delete;
		Window(Window&& w) = delete;
		Window& operator=(Window&& w) = delete;

		GLFWwindow& GetNativeWindow();

		int GetWindowHeight();
		int GetWindowWidth();

		static Window* GetMainWindow();
			
	private:
		friend class Engine;

		/// Called by Engine **********************************

		Window(int width, int height, const char* title);

		void SetResizeWindowEvent(void(*_event)(int, int));
		void SetKeyPressedEvent(void(*_event)(char));
		void SetKeyReleasedEvent(void(*_event)(char));

		void SetMainWindow();

		void PollEvents();
		void SwapBuffers();

		void FpsModeCursor(bool enabled);
		const bool& fpsModeEnabled;

		bool ShouldClose();

		/// End called by Engine ********************************

		GLFWwindow* glfwWindow;

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
	
}

