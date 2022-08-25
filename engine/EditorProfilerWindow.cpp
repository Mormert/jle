// Copyright (c) 2022. Johan Lind

#include "EditorProfilerWindow.h"
#include "3rdparty/ImGui/imgui.h"

jle::EditorProfilerWindow::EditorProfilerWindow(const std::string &window_name)
        : iEditorImGuiWindow{window_name} {}

void jle::EditorProfilerWindow::Update(jle::jleGameEngine &ge) {
    if (!isOpened) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_NoCollapse);

    auto &&vec = jleProfiler::GetProfilerDataLastFrame();

    if (!vec.empty()) {
        DrawProfilerRecursive(0);
    }

    ImGui::End();

}

void jle::EditorProfilerWindow::DrawProfilerRecursive(uint32_t index) {
    auto &&vec = jleProfiler::GetProfilerDataLastFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 15));
    if (ImGui::TreeNodeEx(std::to_string(index).c_str(), ImGuiTreeNodeFlags_DefaultOpen, "%s (%.3f ms)",
                          vec[index].mName.data(),
                          vec[index].mExecutionTime.count() * 0.001f)) {
        for (auto &&childIndex: vec[index].mChildren) {
            DrawProfilerRecursive(childIndex);
        }
        ImGui::TreePop();
    }
    ImGui::PopStyleVar();
}
