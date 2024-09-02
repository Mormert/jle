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

#include "jleEditorSettingsWindow.h"
#include "jleEditor.h"
#include "jleEngineSettings.h"
#include "jleImGuiArchive.h"
#include <ImGui/imgui.h>

jleEditorSettingsWindow::jleEditorSettingsWindow(const std::string &window_name) : jleEditorWindowInterface{window_name}
{
}

void
jleEditorSettingsWindow::renderUI(jleEditorModulesContext &ctx)
{
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    ImGui::BeginChild("settings hierarchy view",
                      ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

    {
        jleImGuiArchive archive{ctx};
        archive(ctx.engine.settings());
    }

    ImGui::EndChild();

    if (ImGui::Button("Save Settings")) {
        jleSerializationContext serializationContext{&ctx.engineModulesContext.resourcesModule,
                                                     &ctx.engineModulesContext.luaEnvironment};
        ctx.engine.settings().saveToFile(serializationContext);
    }

    ImGui::SameLine();
    ImGui::Text("Restart required for settings to take effect.");

    ImGui::End();
}
