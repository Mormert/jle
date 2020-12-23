#pragma once

#include "Window.h"

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

		static int GetMouseX();
		static int GetMouseY();

		static float GetMouseXDelta();
		static float GetMouseYDelta();

		static float GetScrollX();
		static float GetScrollY();

	private:

		friend class Engine;
		friend class internals::Window;

		// Link the input manager to a window
		static void LinkWindow(internals::Window* w);

		// Called from window manager
		static void SetScrollX(float xscrl);
		static void SetScrollY(float yscrl);

		static void UpdateLastMousePosition();
		static void FlushKeyPresses();

		static void KeyPressedEvent(char key);
		static void KeyReleasedEvent(char key);

		static void ResizeWindowEvent(int x, int y);

		template<class T> static void AddResizeWindowCallback(T* const object, void(T::* const mf)(int, int))
		{
			using namespace std::placeholders;
			resizeWindowCallbacks.emplace_back(std::bind(mf, object, _1, _2));
		}

		static std::vector<char> keysPressed;
		static std::vector<char> keysReleased;
		static int lastMouseX, lastMouseY;
		static float scrollX, scrollY;
		static internals::Window* window;
		static std::vector<std::function<void(int, int)>> resizeWindowCallbacks;
	};


}

