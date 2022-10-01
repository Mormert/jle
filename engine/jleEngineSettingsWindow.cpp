// Copyright (c) 2022. Johan Lind

#include "jleEngineSettingsWindow.h"
#include "jleConfigUtils.h"

#include "3rdparty/ImGui/imgui.h"
#include "3rdparty/ImGui/imgui_impl_glfw.h"
#include "3rdparty/ImGui/imgui_impl_opengl3.h"

jleEngineSettingsWindow::jleEngineSettingsWindow(
    const std::string& window_name,
    const std::shared_ptr<jleGameSettings>& gs,
    const std::shared_ptr<jleEditorSettings>& es)
    : iEditorImGuiWindow{window_name}, editorSettings{es} {
    nlohmann::json j_gs = *gs;
    _jsonToImguiGS.JsonToImgui(j_gs, "Game Settings");

    nlohmann::json j_es = *es;
    _jsonToImguiES.JsonToImgui(j_es, "Editor Settings");
}

void jleEngineSettingsWindow::Update(jleGameEngine& ge) {
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    ImGui::BeginChild(
        "settings hierarchy view",
        ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1
                                                         // line below us

    _jsonToImguiGS.DrawAndGetInput();
    _jsonToImguiES.DrawAndGetInput();

    ImGui::EndChild();

    if (ImGui::Button("Save Settings")) {
        auto jsonGS = _jsonToImguiGS.ImGuiToJson();
        cfg::SaveEngineConfig(GAME_RESOURCES_DIRECTORY + "/jle_gs_config.json",
                              jsonGS);

        auto jsonES = _jsonToImguiES.ImGuiToJson();
        cfg::SaveEngineConfig(GAME_RESOURCES_DIRECTORY + "/jle_es_config.json",
                              jsonES);
    }

    ImGui::SameLine();
    ImGui::Text("Restart required for settings to take effect.");

    ImGui::End();
}
