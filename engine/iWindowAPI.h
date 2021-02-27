#pragma once

#include <string>

namespace jle
{
	struct WindowSettings
	{
		std::string WindowTitle{ "jle game engine" };

		unsigned int windowWidth = 500, windowHeight = 500;
		unsigned int windowWidthMin = 100, windowHeightMin = 100;

		bool windowIsRezisable = true;
		bool windowDisplayCursor = true;
	};

	class iWindowAPI
	{
	public:
		virtual ~iWindowAPI() {};

		virtual void SetWindowSettings(WindowSettings& windowSettings) = 0;

		virtual void DisplayCursor(bool enable) = 0;
		virtual bool IsCursorDisplayed() = 0;

		virtual unsigned int GetWindowHeight() = 0;
		virtual unsigned int GetWindowWidth() = 0;

	};
}