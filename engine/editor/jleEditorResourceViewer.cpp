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

#include "jleEditorResourceViewer.h"
#include "core/jleResource.h"
#include "jleGameEngine.h"
#include <ImGui/imgui.h>

void jleEditorResourceViewer::renderUI(jleEngineModulesContext &ctx) {
    if (!isOpened) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_NoCollapse);

    auto &resources = ctx.resourcesModule;

    std::vector<jlePath> resourcesToBeUnloaded;
    for (auto &&drive : resources.resourcesMap()) {
        const std::string treeNodeStr =
            drive.first + " (" + std::to_string(drive.second.size()) + ")";

        const bool open = ImGui::TreeNodeEx(treeNodeStr.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::Button("Unload Everything")) {
                resources.unloadAllResources(drive.first);
            }
            ImGui::EndPopup();
        }
        if (open) {

            for (auto &&path : drive.second) {
                const std::string pathStr =
                    path.first.getVirtualPath() + " (" +
                    std::to_string(path.second.second.use_count() - 1) + " users)";

                if (ImGui::TreeNode(pathStr.c_str())) {
                    if (ImGui::Button("Unload")) {
                        resourcesToBeUnloaded.push_back(path.first);
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }

    // Postpone the unloading to outside the loop
    for (auto &&resourcePath : resourcesToBeUnloaded) {
        resources.unloadResource(resourcePath);
    }

    ImGui::End();
}

jleEditorResourceViewer::jleEditorResourceViewer(const std::string &window_name)
    : jleEditorWindowInterface{window_name} {}

void
jleEditorResourceViewer::update(jleResources &resourcesModule)
{
    if (!isOpened) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_NoCollapse);

    auto &resources = resourcesModule;

    std::vector<jlePath> resourcesToBeUnloaded;
    for (auto &&drive : resources.resourcesMap()) {
        const std::string treeNodeStr =
            drive.first + " (" + std::to_string(drive.second.size()) + ")";

        const bool open = ImGui::TreeNodeEx(treeNodeStr.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::Button("Unload Everything")) {
                resources.unloadAllResources(drive.first);
            }
            ImGui::EndPopup();
        }
        if (open) {

            for (auto &&path : drive.second) {
                const std::string pathStr =
                    path.first.getVirtualPath() + " (" +
                    std::to_string(path.second.second.use_count() - 1) + " users)";

                if (ImGui::TreeNode(pathStr.c_str())) {
                    if (ImGui::Button("Unload")) {
                        resourcesToBeUnloaded.push_back(path.first);
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }

    // Postpone the unloading to outside the loop
    for (auto &&resourcePath : resourcesToBeUnloaded) {
        resources.unloadResource(resourcePath);
    }

    ImGui::End();
}
