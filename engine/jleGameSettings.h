// Copyright (c) 2023. Johan Lind

#pragma once

#include "json.hpp"
#include <iostream>

#include "jleCoreSettings.h"
#include "plog/Log.h"

struct jleGameSettings : public jleCoreSettings {
    /*struct jleFramebufferSettings {
        FIXED_AXIS fixedAxis{FIXED_AXIS::same_as_window};
        unsigned int fixedAxisPixels{0};
    } framebufferSettings;*/

    ~jleGameSettings() override = default;
};

/*#pragma region jleFramebufferSettings to / from json
using json = nlohmann::json;

#pragma region FramebufferSettings to / from json

inline void to_json(nlohmann::json &j,
                    const jleGameSettings::jleFramebufferSettings &fs) {
    j = json{{"fixed_axis_enum", fs.fixedAxis},
             {"fixed_axis_pixels", fs.fixedAxisPixels}};
}

inline void from_json(const nlohmann::json &j,
                      jleGameSettings::jleFramebufferSettings &fs) {
    j.at("fixed_axis_enum").get_to(fs.fixedAxis);
    j.at("fixed_axis_pixels").get_to(fs.fixedAxisPixels);
}

#pragma endregion

#pragma region GameSettings to / from json

inline void to_json(nlohmann::json &j, const jleGameSettings &gs) {
    j = (jleCoreSettings &)gs;
    j["framebuffer_settings"] = gs.framebufferSettings;
}

inline void from_json(const nlohmann::json &j, jleGameSettings &gs) {
    from_json(j, (jleCoreSettings &)gs);
    j.at("framebuffer_settings").get_to(gs.framebufferSettings);
}

#pragma endregion*/
