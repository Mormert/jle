#pragma once

#include "JLE_ENGINE_CONFIG.h"

#include "3rdparty/json.hpp"

#include <string>
#include <functional>

namespace jle
{
	struct WindowSettings
	{
		std::string WindowTitle{ "jle game engine" };

		unsigned int windowWidth = 500, windowHeight = 500;
		unsigned int windowWidthMin = 100, windowHeightMin = 100;

		std::string iconPath{ JLE_ENGINE_PATH + "/EditorResources/jle_default_icon.png" };

		bool windowIsRezisable = true;
		bool windowDisplayCursor = true;
	};

	using json = nlohmann::json;
	inline void to_json(json& j, const WindowSettings& ws) {
		j = json{
			{"window_title", ws.WindowTitle},
			{"window_width", ws.windowWidth},
			{"window_height", ws.windowHeight},
			{"window_width_min", ws.windowWidthMin},
			{"window_height_min", ws.windowHeightMin},
			{"window_icon_path", ws.iconPath},
			{"window_is_rezisable", ws.windowIsRezisable},
			{"window_display_cursor", ws.windowDisplayCursor}
		};
	}

	inline void from_json(const json& j, WindowSettings& ws) {
		j.at("window_title").get_to(ws.WindowTitle);
		j.at("window_width").get_to(ws.windowWidth);
		j.at("window_height").get_to(ws.windowHeight);
		j.at("window_width_min").get_to(ws.windowWidthMin);
		j.at("window_height_min").get_to(ws.windowHeightMin);
		j.at("window_icon_path").get_to(ws.iconPath);
		j.at("window_is_rezisable").get_to(ws.windowIsRezisable);
		j.at("window_display_cursor").get_to(ws.windowDisplayCursor);
	}

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