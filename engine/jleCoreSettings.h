// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleWindowSettings.h"
#include "jleEngineInternalAPIs.h"

namespace jle {
    struct jleCoreSettings {
        WindowSettings windowSettings;
        jleEngineInternalAPISettings engineAPIs;

        virtual ~jleCoreSettings() {}
    };

#pragma region CoreSettings to/from json
    using json = nlohmann::json;

    inline void to_json(nlohmann::json &j, const jleCoreSettings &cs) {
        j = json{{"window_settings",     cs.windowSettings},
                 {"engine_abstractions", cs.engineAPIs}};
    }

    inline void from_json(const nlohmann::json &j, jleCoreSettings &cs) {
        j.at("window_settings").get_to(cs.windowSettings);
        j.at("engine_abstractions").get_to(cs.engineAPIs);
    }

#pragma endregion

}