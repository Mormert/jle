// Copyright (c) 2022. Johan Lind

#include "jleEditorProfilerWindow.h"
#include "ImGui/imgui.h"

jleEditorProfilerWindow::jleEditorProfilerWindow(const std::string& window_name)
    : iEditorImGuiWindow{window_name} {}

void jleEditorProfilerWindow::update(jleGameEngine& ge) {
    if (!isOpened) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_NoCollapse);

    auto&& vec = jleProfiler::profilerDataLastFrame();

    if (!vec.empty()) {
        drawProfilerRecursive(0);
    }

    ImGui::End();
}

void jleEditorProfilerWindow::drawProfilerRecursive(uint32_t index) {
    auto&& vec = jleProfiler::profilerDataLastFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 15));
    if (ImGui::TreeNodeEx(std::to_string(index).c_str(),
                          ImGuiTreeNodeFlags_DefaultOpen,
                          "%s (%.3f ms)",
                          vec[index]._name.data(),
                          vec[index]._executionTime.count() * 0.001f)) {
        for (auto&& childIndex : vec[index]._children) {
            drawProfilerRecursive(childIndex);
        }
        ImGui::TreePop();
    }
    ImGui::PopStyleVar();
}
