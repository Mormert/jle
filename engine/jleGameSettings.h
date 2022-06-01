// Copyright (c) 2022. Johan Lind

#pragma once

#include <iostream>
#include "3rdparty/json.hpp"

#include "jleCoreSettings.h"
#include "plog/Log.h"

namespace jle {
    // Specifies which axis to be fixed, and the other scales depending on window aspect ratio
    enum class FIXED_AXIS {
        same_as_window, width, height
    };

    struct jleGameSettings : public jleCoreSettings {
        struct jleFramebufferSettings {
            FIXED_AXIS fixedAxis{FIXED_AXIS::same_as_window};
            unsigned int fixedAxisPixels{0};
        } framebufferSettings;

        ~jleGameSettings() override = default;
    };

#pragma region jleFramebufferSettings to/from json
    using json = nlohmann::json;

    inline void to_json(nlohmann::json &j, const jleGameSettings::jleFramebufferSettings &fs) {
        j = json{
                {"fixed_axis_enum",   fs.fixedAxis},
                {"fixed_axis_pixels", fs.fixedAxisPixels}
        };
    }

    inline void from_json(const nlohmann::json &j, jleGameSettings::jleFramebufferSettings &fs) {
        j.at("fixed_axis_enum").get_to(fs.fixedAxis);
        j.at("fixed_axis_pixels").get_to(fs.fixedAxisPixels);
    }

#pragma endregion

#pragma region GameSettings to/from json

    inline void to_json(nlohmann::json &j, const jleGameSettings &gs) {
        j = (jleCoreSettings &) gs;
        j["framebuffer_settings"] = gs.framebufferSettings;
    }

    inline void from_json(const nlohmann::json &j, jleGameSettings &gs) {
        from_json(j, (jleCoreSettings &) gs);
        j.at("framebuffer_settings").get_to(gs.framebufferSettings);
    }

#pragma endregion

}