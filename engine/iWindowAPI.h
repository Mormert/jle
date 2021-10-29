#pragma once

#include "jleWindowSettings.h"

#include "3rdparty/json.hpp"

#include <string>
#include <functional>

namespace jle
{
	class iWindowAPI
	{
	public:
		virtual ~iWindowAPI() {};

		virtual void SetWindowSettings(WindowSettings& windowSettings) = 0;

		// Adds a function to be called when window is resized. Returns an id that can be used when removing the callback
		virtual unsigned int AddWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback) = 0;

		// Removes a resize callback with the id retrieved from the add callback method
		virtual void RemoveWindowResizeCallback(unsigned int callback_id) = 0;


		virtual void DisplayCursor(bool enable) = 0;
		virtual bool IsCursorDisplayed() = 0;

		virtual unsigned int GetWindowHeight() = 0;
		virtual unsigned int GetWindowWidth() = 0;

	};
}