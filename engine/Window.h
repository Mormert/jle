#pragma once

#include <memory>

namespace jle
{
	
	// Pimpl idiom, class definition exsists in Window.cpp
	class WindowImpl;

	class Window final
	{
	public:

		~Window();

		// No copy, no move
		Window(const Window &w) = delete;
		Window& operator=(const Window &w) = delete;
		Window(Window&& w) = delete;
		Window& operator=(Window&& w) = delete;

		// Get the window's height in pixels
		int GetWindowHeight();

		// Get the window's width in pixels
		int GetWindowWidth();

		// Get the main window
		static Window* GetMainWindow();
		
		// Returns true if a key is pressed within the specified Window
		friend bool WindowGetKey(const Window &window, char key);
		// Sets referenced x & y values to cursor position for specified Window
		friend void WindowGetCursorPosition(const Window& window, double &x, double &y);

	private:
		friend class Engine;

		/// Called by Engine **********************************
			Window(int width, int height, const char* title);

			// Sets what function with parameters (int, int) should be called when window is rezised
			void SetResizeWindowEvent(void(*event_)(int, int));
			// Sets what function with parameters (char) should be called when a key is pressed in the window
			void SetKeyPressedEvent(void(*event_)(char));
			// Sets what function with parameters (char) shoudl be called when a key is released in the window
			void SetKeyReleasedEvent(void(*event_)(char));

			// Sets this Window to be the main window (currently no support for several Windows!)
			void SetMainWindow();

			// Polls the events from input devices such as the mouse and keyboard
			void PollEvents();
			// Swaps the double buffers for the gl rendering
			void SwapBuffers();

			// Set the Window to hide the cursor, defaults to false
			void FpsModeCursor(bool enabled);
			const bool& fpsModeEnabled;

			// Returns true when the window has been closed by the user for example by pressing the X button
			bool ShouldClose();
		/// End called by Engine ********************************

		// The window's width in pixels
		int window_width;
		// The window's height in pixels
		int window_height;

		// Client tracking of if FPS mode is enabled or not
		bool fpsModeEnabled_{ false };

		static Window* mainWindow;


		// Pimpl idiom implementation
		friend class WindowImpl;
		// Gets a const version of the pImpl
		const WindowImpl* Pimpl() const { return pImpl.get(); };
		// Gets the Window Pimpl class
		WindowImpl* Pimpl() { return pImpl.get(); };

		// Smart pointer to the implementation (exsists in Window.cpp)
		std::unique_ptr<WindowImpl> pImpl;

	};
	
}

