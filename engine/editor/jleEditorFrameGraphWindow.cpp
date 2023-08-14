// Copyright (c) 2023. Johan Lind

#include "jleEditorFrameGraphWindow.h"
#include "implot/implot.h"

jleEditorFrameGraphWindow::jleEditorFrameGraphWindow(const std::string &window_name)
    : jleEditorWindowInterface(window_name)
{
}

void
jleEditorFrameGraphWindow::update(jleGameEngine &ge)
{
    static std::vector<float> data{};
    static bool enabled = false;

    ImGui::Begin("Frame Graph");
    {
        ImGui::Checkbox("Enable Frame Time Plotting", &enabled);
        ImGui::SameLine();
        if (ImGui::Button("Clear Plot")) {
            data.clear();
        }

        if (enabled) {
            data.push_back(ge.deltaFrameTime() * 1000.f);
        }

        ImPlot::SetNextAxesLimits(-25.0, 175.0, -25.0, 175.0, ImGuiCond_FirstUseEver);
        if (ImPlot::BeginPlot("Frame Plot")) {
            if(data.size() > 0)
            {
                ImPlot::PlotBars("Frame Time (ms)", &data[0], data.size());
            }
            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}
