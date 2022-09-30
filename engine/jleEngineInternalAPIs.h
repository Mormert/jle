// Copyright (c) 2022. Johan Lind

#include "json.hpp"

#pragma once

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

#pragma endregion