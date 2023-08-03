// Copyright (c) 2023. Johan Lind

#include "jleEditorSettingsWindow.h"
#include "ImGui/imgui.h"
#include "jleEditor.h"
#include "jleEngineSettings.h"
#include "jleImGuiCerealArchive.h"

jleEditorSettingsWindow::jleEditorSettingsWindow(const std::string &window_name) : jleEditorWindowInterface{window_name} {}

void
jleEditorSettingsWindow::update(jleGameEngine &ge)
{
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    ImGui::BeginChild("settings hierarchy view",
                      ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

    cereal::jleImGuiCerealArchive archive;
    archive(gEngine->settings());

    ImGui::EndChild();

    if (ImGui::Button("Save Settings")) {
        gEngine->settings().saveToFile();
    }

    ImGui::SameLine();
    ImGui::Text("Restart required for settings to take effect.");

    ImGui::End();
}
