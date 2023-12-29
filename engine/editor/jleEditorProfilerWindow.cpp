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

#include "jleEditorProfilerWindow.h"
#include <ImGui/imgui.h>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif

jleEditorProfilerWindow::jleEditorProfilerWindow(const std::string &window_name) : jleEditorWindowInterface{window_name} {}

void
jleEditorProfilerWindow::update(jleGameEngine &ge)
{
    if (!isOpened) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_NoCollapse);

    auto &&vec = jleProfiler::profilerDataLastFrame();

    if (!vec.empty()) {
        drawProfilerRecursive(0);
    }

    ImGui::Separator();

    ImGui::Text("Remotery profiling disabled. Enable by compiling with -DBUILD_REMOTERY");

    ImGui::End();
}

void
jleEditorProfilerWindow::drawProfilerRecursive(uint32_t index)
{
    auto &&vec = jleProfiler::profilerDataLastFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 15));
    if (ImGui::TreeNodeEx(std::to_string(index).c_str(),
                          ImGuiTreeNodeFlags_DefaultOpen,
                          "%s (%.3f ms)",
                          vec[index]._name.data(),
                          vec[index]._executionTime.count() * 0.001f)) {
        for (auto &&childIndex : vec[index]._children) {
            drawProfilerRecursive(childIndex);
        }
        ImGui::TreePop();
    }
    ImGui::PopStyleVar();
}
