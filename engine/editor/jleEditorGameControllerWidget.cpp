// Copyright (c) 2022. Johan Lind

#include "jleEditorGameControllerWidget.h"

#include "3rdparty/ImGui/imgui.h"

#include "3rdparty/git_submodules/plog/include/plog/Log.h"

jleEditorGameControllerWidget::jleEditorGameControllerWidget(
    const std::string& window_name)
    : iEditorImGuiWindow{window_name} {
    mPlayGameIcon = jleCore::core->texture_creator->LoadTextureFromPath(
        jleRelativePath{"ED:/icons/play.png"});
    mRestartGameIcon = jleCore::core->texture_creator->LoadTextureFromPath(
        jleRelativePath{"ED:/icons/replay.png"});
    mPauseGameIcon = jleCore::core->texture_creator->LoadTextureFromPath(
        jleRelativePath{"ED:/icons/pause.png"});
    mNextFrameIcon = jleCore::core->texture_creator->LoadTextureFromPath(
        jleRelativePath{"ED:/icons/next_frame.png"});
}

void jleEditorGameControllerWidget::Update(jleGameEngine& ge) {

    const ImVec2 iconSize{ImGui::GetWindowHeight() - 3,
                          ImGui::GetWindowHeight() - 3};

    if (ge.IsGameKilled()) {
        if (ImGui::Button(("Start Game"))) {
            LOG_VERBOSE << "Starting the game.";
            ge.StartGame();
        }
    }
    else {
        if (ImGui::Button(("Restart Game"))) {
            LOG_VERBOSE << "Restarting the game.";
            ge.RestartGame();
        }
    }

    ImGui::SameLine();

    if (!ge.IsGameHalted()) {
        if (ImGui::Button(("Pause Game"))) {
            LOG_VERBOSE << "Pausing the game.";
            ge.HaltGame();
        }
    }
    else {
        if (ImGui::Button(("Continue Game"))) {
            LOG_VERBOSE << "Continue playing the game.";
            ge.UnhaltGame();
        }
        ImGui::SameLine();
        if (ImGui::Button(("Skip Frame"))) {
            LOG_VERBOSE << "Going one frame forward.";
            if (!ge.IsGameKilled()) {
                ge.ExecuteNextFrame();
            }
        }
    }
}