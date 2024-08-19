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

#include "jleEditorResourceEdit.h"
#include "jleGameEngine.h"

#include "editor/jleImGuiCerealArchive.h"

jleEditorResourceEdit::jleEditorResourceEdit(const std::string &window_name) : jleEditorWindowInterface(window_name) {}

void
jleEditorResourceEdit::renderUI(jleSerializationContext &ctx)
{
    std::vector<jlePath> toBeRemoved{};

    for (auto &[path, resource] : _resources) {

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowSize(ImVec2(500, 700), ImGuiCond_FirstUseEver);
        bool shouldBeOpened = true;
        ImGui::Begin(std::string{"Resource: " + path.getVirtualPath()}.c_str(), &shouldBeOpened, flags);

        if (!shouldBeOpened) {
            toBeRemoved.push_back(path);
        }

        ImGui::BeginChild("resource hierarchy view",
                          ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

        {
            cereal::jleImGuiCerealArchiveInternal ar1{ctx};
            ar1(resource);
        }

        ImGui::EndChild();

        if (resource && ImGui::Button("Save Resource")) {
            resource->saveToFile();
        }

        ImGui::SameLine();

        if (resource && ImGui::Button("Reload Resource")) {
            ctx.resources.reloadSerializedResource(resource);
        }

        ImGui::End();
    }

    for (auto &&path : toBeRemoved) {
        _resources.erase(path);
    }
}

bool
jleEditorResourceEdit::tryOpen(const jlePath &path, jleResources &resources)
{
    if (_resources.find(path) != _resources.end()) {
        return false;
    }

    try {
        auto resource = resources.loadSerializedResourceFromFile(path);
        _resources.insert(std::make_pair(path, resource));
    } catch (std::exception &e) {
        LOGV << "Failed to open resource with path: " << path.getVirtualPath() << ", error: " << e.what();
        return false;
    }

    return true;
}
