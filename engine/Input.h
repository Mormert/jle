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

		// Returns true if a specified key is pressed
		static bool GetKeyPressed(char key);
		// Returns true if a specified key is released
		static bool GetKeyReleased(char key);
		// Returns true if a speicifed key is down
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
		friend class WindowImpl;

		// Link the input manager to a window
		static void LinkWindow(Window* w);

		// Link the input manager to a viewport
		static void LinkViewport(Viewport* vp);

		// Called from Window (WindowImpl)
		static void SetScrollX(float xscrl);
		// Called from Window (WindowImpl)
		static void SetScrollY(float yscrl);

		// Called from Engine
		static void UpdateLastMousePosition();
		// Called from Engine
		static void FlushKeyPresses();

		// Should be called when a key is registered to be pressed
		static void KeyPressedEvent(char key);
		// Should be called when a key is registered to be released
		static void KeyReleasedEvent(char key);

		// Should be called when a window has been resized, it forwards the data to
		// all callbacks added via AddResizeWindowCallback.
		static void ResizeWindowEvent(int x, int y);

		template<class T> static void AddResizeWindowCallback(T* const object, void(T::* const mf)(unsigned int, unsigned int))
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

