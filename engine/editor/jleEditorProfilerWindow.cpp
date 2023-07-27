// Copyright (c) 2023. Johan Lind

#include "jleEditorProfilerWindow.h"
#include "ImGui/imgui.h"

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif

jleEditorProfilerWindow::jleEditorProfilerWindow(const std::string &window_name) : iEditorImGuiWindow{window_name} {}

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

#if RMT_ENABLED
    if (ImGui::Button("Open Remotery Profiling")) {
#ifdef WIN32
        const std::string remoteryVisHtml = JLE_ENGINE_PATH + "3rdparty/Remotery/vis/index.html";
        ShellExecute(NULL, NULL, remoteryVisHtml.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
        const std::string systemOpen = "open " + JLE_ENGINE_PATH + "3rdparty/Remotery/vis/index.html";
        system(systemOpen.c_str());
#endif
    }
#else
    ImGui::Text("Remotery profiling disabled. Enable by compiling with -DBUILD_REMOTERY");
#endif

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
