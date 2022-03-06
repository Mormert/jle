#pragma once

#include "jlePathDefines.h"

#include "3rdparty/json.hpp"

#include <string>

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
}