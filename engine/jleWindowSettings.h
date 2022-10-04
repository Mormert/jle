// Copyright (c) 2022. Johan Lind

#pragma once

#include "jlePathDefines.h"

#include "json.hpp"

#include <string>

struct WindowSettings {
    std::string WindowTitle{"jle game engine"};

    unsigned int width = 500, height = 500;
    unsigned int widthMin = 100, heightMin = 100;

    std::string iconPath{JLE_ENGINE_PATH +
                         "/EditorResources/jle_default_icon.png"};

    bool isRezisable = true;
    bool shouldDisplayCursor = true;
    bool isCappedFps = false;
};

using json = nlohmann::json;

inline void to_json(json &j, const WindowSettings &ws) {
    j = json{{"window_title", ws.WindowTitle},
             {"window_width", ws.width},
             {"window_height", ws.height},
             {"window_width_min", ws.widthMin},
             {"window_height_min", ws.heightMin},
             {"window_icon_path", ws.iconPath},
             {"window_is_rezisable", ws.isRezisable},
             {"window_display_cursor", ws.shouldDisplayCursor},
             {"windowCappedFps", ws.isCappedFps}};
}

inline void from_json(const json &j, WindowSettings &ws) {
    j.at("window_title").get_to(ws.WindowTitle);
    j.at("window_width").get_to(ws.width);
    j.at("window_height").get_to(ws.height);
    j.at("window_width_min").get_to(ws.widthMin);
    j.at("window_height_min").get_to(ws.heightMin);
    j.at("window_icon_path").get_to(ws.iconPath);
    j.at("window_is_rezisable").get_to(ws.isRezisable);
    j.at("window_display_cursor").get_to(ws.shouldDisplayCursor);
    j.at("window_display_cursor").get_to(ws.shouldDisplayCursor);
    j.at("windowCappedFps").get_to(ws.isCappedFps);
}
