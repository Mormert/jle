#pragma once

#include "Window.h"
#include "Viewport.h"

#include <functional>
#include <vector>

namespace jle
{
	class Input
	{
	public:

		static bool GetKeyPressed(char key);
		static bool GetKeyReleased(char key);
		static bool GetKeyDown(char key);

		// Gets the real X mouse position on the Window
		static int GetMouseXRaw() noexcept;
		// Gets the real Y mouse position on the Window
		static int GetMouseYRaw() noexcept;

		// Gets the X mouse position 
		static int GetMouseX() noexcept;
		// Gets the X mouse position 
		static int GetMouseY() noexcept;

		// Gets the X position in the world where the mouse hovers over
		static int GetMouseWorldX() noexcept;
		// Gets the Y position in the world where the mouse hovers over
		static int GetMouseWorldY() noexcept;

		// Gets the real X mouse delta for 1 frame
		static float GetMouseXRawDelta() noexcept;
		// Gets the real Y mouse delta for 1 frame
		static float GetMouseYRawDelta() noexcept;

		// Returns a float != 0 if scroll detected, value depend on scroll
		static float GetScrollX() noexcept;
		// Returns a float != 0 if scroll detected, value depend on scroll
		static float GetScrollY() noexcept;

	private:

		friend class Engine;
		friend class Window;

		// Link the input manager to a window
		static void LinkWindow(Window* w);

		// Link the input manager to a viewport
		static void LinkViewport(Viewport* vp);

		// Called from window manager
		static void SetScrollX(float xscrl);
		static void SetScrollY(float yscrl);

		static void UpdateLastMousePosition();
		static void FlushKeyPresses();

		static void KeyPressedEvent(char key);
		static void KeyReleasedEvent(char key);

		static void ResizeWindowEvent(int x, int y);

		template<class T> static void AddResizeWindowCallback(T* const object, void(T::* const mf)(unsigned int,unsigned int))
		{
			using namespace std::placeholders;
			resizeWindowCallbacks.emplace_back(std::bind(mf, object, _1, _2));
		}

		static std::vector<char> keysPressed;
		static std::vector<char> keysReleased;
		static int lastMouseX, lastMouseY;
		static float scrollX, scrollY;
		static Window* window;
		static Viewport* viewport;
		static std::vector<std::function<void(int, int)>> resizeWindowCallbacks;
	};


}

