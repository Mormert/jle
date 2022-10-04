// Copyright (c) 2022. Johan Lind

#include "jleEditorWindowsPanel.h"

#include "jleGameEditorWindow.h"

#include "ImGui/imgui.h"
#include "plog/Log.h"

jleEditorWindowsPanel::jleEditorWindowsPanel(const std::string& window_name)
    : iEditorImGuiWindow{window_name}, _gameController{"Game Controller"} {}

void jleEditorWindowsPanel::update(jleGameEngine& ge) { dockspaceupdate(ge); }

void jleEditorWindowsPanel::addWindow(
    std::shared_ptr<iEditorImGuiWindow> window) {
    windows.push_back(window);
}

void jleEditorWindowsPanel::dockspaceupdate(jleGameEngine& ge) {

    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    bool open = true;
    bool *p_open = &open;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoNavFocus;
    }
    else {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
    // render our background and handle the pass-thru hole, so we ask Begin() to
    // not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window
    // is collapsed). This is because we want to keep our DockSpace() active. If
    // a DockSpace() is inactive, all active windows docked into it will lose
    // their parent and become undocked. We cannot preserve the docking
    // relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in
    // limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", p_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("DockSpaceID");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    menuButtonsupdate(ge);

    ImGui::End();
}

void jleEditorWindowsPanel::menuButtonsupdate(jleGameEngine& ge) {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Manage Windows")) {
            for (auto&& window : windows) {
                if (window->opened()) {
                    if (ImGui::MenuItem(
                            ("Close " + window->windowName()).c_str())) {
                        window->closeWindow();
                    }
                }
                else {
                    if (ImGui::MenuItem(
                            ("Open " + window->windowName()).c_str())) {
                        window->openWindow();
                    }
                }
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Game Controller")) {
            _gameController.update(ge);
            ImGui::EndMenu();
        }

        auto windowWidth = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((windowWidth)*0.5f);

        ImGui::SetCursorPosX(
            ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -
            ImGui::CalcTextSize("FPS: XXXX, DT: XXXXXXXX, Time: XXXXX").x -
            ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
        ImGui::Text("FPS: %4d, DT: %4f, Time: %4f",
                    ge.status->fps(),
                    ge.status->deltaFrameTime(),
                    ge.status->currentFrameTime());

        ImGui::EndMenuBar();
    }
}
