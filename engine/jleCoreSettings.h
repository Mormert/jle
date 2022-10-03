// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleWindowSettings.h"
#include "json.hpp"

struct jleEngineInternalAPISettings {
    enum class RenderingAPI {
        OPENGL_33
    } renderingAPI = RenderingAPI::OPENGL_33;
    enum class WindowAPI { GLFW } windowingAPI = WindowAPI::GLFW;
};

#pragma region EngineInternalAPIs to / from json
using json = nlohmann::json;

inline void to_json(nlohmann::json& j, const jleEngineInternalAPISettings& ei) {
    j = json{{"rendering_api", ei.renderingAPI},
             {"windowing_api", ei.windowingAPI}};
}

inline void from_json(const nlohmann::json& j,
                      jleEngineInternalAPISettings& ei) {
    j.at("rendering_api").get_to(ei.renderingAPI);
    j.at("windowing_api").get_to(ei.windowingAPI);
}

struct jleCoreSettings {
    WindowSettings windowSettings;
    jleEngineInternalAPISettings engineAPIs;

    virtual ~jleCoreSettings() {}
};

#pragma region CoreSettings to / from json
using json = nlohmann::json;

inline void to_json(nlohmann::json& j, const jleCoreSettings& cs) {
    j = json{{"window_settings", cs.windowSettings},
             {"engine_abstractions", cs.engineAPIs}};
}

inline void from_json(const nlohmann::json& j, jleCoreSettings& cs) {
    j.at("window_settings").get_to(cs.windowSettings);
    j.at("engine_abstractions").get_to(cs.engineAPIs);
}

#pragma endregion
