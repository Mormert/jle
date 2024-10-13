/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleEditorGameControllerWidget.h"

#include "core/jleResource.h"

#include <ImGui/imgui.h>

#include <plog/Log.h>

jleEditorGameControllerWidget::jleEditorGameControllerWidget(
    const std::string &window_name)
    : jleEditorWindowInterface{window_name} {
}

void jleEditorGameControllerWidget::render(jleEngineModulesContext& ctx) {

    const ImVec2 iconSize{ImGui::GetWindowHeight() - 3,
                          ImGui::GetWindowHeight() - 3};

    if (ctx.gameRuntime.isGameKilled()) {
        if (ImGui::Button(("Start Game"))) {
            LOG_VERBOSE << "Starting the game.";
            ctx.gameRuntime.startGame(ctx);
        }
    }
    else {
        if (ImGui::Button(("Restart Game"))) {
            LOG_VERBOSE << "Restarting the game.";
            ctx.gameRuntime.restartGame(ctx);
        }
        if (ImGui::Button(("Kill Game"))) {
            LOG_VERBOSE << "Killing the game.";
            ctx.gameRuntime.killGame();
        }
    }

    ImGui::SameLine();

    if (!ctx.gameRuntime.isGameHalted()) {
        if (ImGui::Button(("Pause Game"))) {
            LOG_VERBOSE << "Pausing the game.";
            ctx.gameRuntime.haltGame();
        }
    }
    else {
        if (ImGui::Button(("Continue Game"))) {
            LOG_VERBOSE << "Continue playing the game.";
            ctx.gameRuntime.unhaltGame();
        }
        ImGui::SameLine();
        if (ImGui::Button(("Skip Frame"))) {
            LOG_VERBOSE << "Going one frame forward.";
            if (!ctx.gameRuntime.isGameKilled()) {
                ctx.gameRuntime.executeNextFrame(ctx);
            }
        }
    }
}
