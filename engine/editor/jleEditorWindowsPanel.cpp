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

#include "jleEditorWindowsPanel.h"

#include "jleEngineSettings.h"
#include "jleGameEditorWindow.h"
#include "modules/windowing/jleWindow.h"

#include <ImGui/imgui.h>
#include <plog/Log.h>


jleEditorWindowsPanel::jleEditorWindowsPanel(const std::string &window_name, jleResources& resources, jleEngineSettings& settings)
    : jleEditorWindowInterface{window_name}, _gameController{"Game Controller"}
{
    _crossIcon = resources.loadResourceFromFile<jleTexture>(jlePath{"ED:/icons/cross.png"});
    _maximizeIcon = resources.loadResourceFromFile<jleTexture>(jlePath{"ED:/icons/maximize.png"});
    _minimizeIcon = resources.loadResourceFromFile<jleTexture>(jlePath{"ED:/icons/minimize.png"});
    _jleIcon = resources.loadResourceFromFile<jleTexture>(settings.windowSettings.iconPath);
}

void
jleEditorWindowsPanel::renderUI(const jleEditorWindowsPanelRenderContext& context)
{
    dockspaceupdate(context);
}

void
jleEditorWindowsPanel::addWindow(std::shared_ptr<jleEditorWindowInterface> window)
{
    windows.push_back(window);
}

void
jleEditorWindowsPanel::dockspaceupdate(const jleEditorWindowsPanelRenderContext& context)
{

    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    bool open = true;
    bool *p_open = &open;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    } else {
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
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("DockSpaceID");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    menuButtonsupdate(context);

    ImGui::End();
}

void
jleEditorWindowsPanel::menuButtonsupdate(const jleEditorWindowsPanelRenderContext& context)
{
    if (ImGui::BeginMenuBar()) {

        const float menuBarWidth = ImGui::GetWindowWidth();
        const float menuBarHeight = ImGui::GetFrameHeightWithSpacing();

        ImGui::SetCursorPosX(1.f);

        const float iconDimWidthHeight = std::max(_jleIcon->width() * 0.5f, menuBarHeight);
        ImGui::Image((void *)(intptr_t)_jleIcon->id(), ImVec2{iconDimWidthHeight, iconDimWidthHeight});

        if (ImGui::BeginMenu("Manage Windows")) {
            for (auto &&window : windows) {
                if (window->opened()) {
                    if (ImGui::MenuItem(("Close " + window->windowName()).c_str())) {
                        window->closeWindow();
                    }
                } else {
                    if (ImGui::MenuItem(("Open " + window->windowName()).c_str())) {
                        window->openWindow();
                    }
                }
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Game Controller")) {
            _gameController.render(context.gameEngine);
            ImGui::EndMenu();
        }

        auto windowWidth = ImGui::GetWindowSize().x;
        const float textOffset = ImGui::CalcTextSize("Avg FPS: XXXX  |  Run Time: HH:MM:SS.MMM").x * 0.5f;
        ImGui::SetCursorPosX((windowWidth) * 0.5f - textOffset);

        // clang-format off
        const auto formatTime = [](int value)
        {
            std::string result=""; uint8_t n=value/86400000;
            if (n>0) {result=std::to_string(n)+' ';}
            value-=86400000*n; n=value/3600000; if(n<10) result+='0';
            result+=std::to_string(n); value-=3600000*n;
            n=value/60000; result+=':'; if(n<10) result+='0';
            result+=std::to_string(n); value-=60000*n;
            n=value/1000; result+=':'; if(n<10) result+='0';
            result+=std::to_string(n); value-=1000*n;
            result+='.'; if(value<100) result+='0';
            if(value<10) result+='0';
            result+=std::to_string(value);
            return result;
        };
        // clang-format on

        const auto rolling120FramesAvgFps = (int)ImGui::GetIO().Framerate;
        ImGui::Text("Avg FPS: %4d  |  Run Time: %s",
                    rolling120FramesAvgFps,
                    formatTime(static_cast<int>(context.frameInfo.getCurrentFrameTime() * 1000.f)).c_str());

        {
            ImGuiStyle &style = ImGui::GetStyle();

            const float originalFrameRounding = style.FrameRounding;
            const ImVec2 originalItemSpacing = style.ItemSpacing;

            // Set FrameRounding to 0 to create sharp corners
            style.FrameRounding = 0.0f;

            // Set ItemSpacing to 0 to remove the gap between buttons
            style.ItemSpacing = ImVec2(0, style.ItemSpacing.y);

            const ImVec4 menuBarColor = style.Colors[ImGuiCol_MenuBarBg];
            const ImVec4 buttonHoveredColor = ImVec4(menuBarColor.x * 1.1f,
                                                     menuBarColor.y * 1.1f,
                                                     menuBarColor.z * 1.1f,
                                                     menuBarColor.w);

            const float ratio = 0.5f * menuBarHeight / _crossIcon->height();
            const ImVec2 buttonSize{(float)_crossIcon->width() * ratio, (float)_crossIcon->height() * ratio};
            const float buttonWidth = buttonSize.x + style.FramePadding.x * 2;
            const float totalButtonWidth = 3 * buttonWidth;
            const float offset = menuBarWidth - totalButtonWidth - style.ItemSpacing.x * 2;

            ImGui::SetCursorPosX(offset);

            {
                ImGui::PushStyleColor(ImGuiCol_Button, menuBarColor);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonHoveredColor);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

                if (ImGui::ImageButton((void *)(intptr_t)_minimizeIcon->id(), buttonSize)) {
                    glfwIconifyWindow(context.window.glfwWindow());
                }

                if (ImGui::ImageButton((void *)(intptr_t)_maximizeIcon->id(), buttonSize)) {
                    const auto window = context.window.glfwWindow();
                    if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) {
                        glfwRestoreWindow(window);
                    } else {
                        glfwMaximizeWindow(window);
                    }
                }

                ImVec4 grayRedHoveredColor = ImVec4(0.7f, 0.2f, 0.2f, 1.0f);
                {
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, grayRedHoveredColor);

                    if (ImGui::ImageButton((void *)(intptr_t)_crossIcon->id(), buttonSize)) {
                        glfwSetWindowShouldClose(context.window.glfwWindow(), true);
                    }
                    ImGui::PopStyleColor();
                }

                // Pop the style colors
                ImGui::PopStyleColor(4);
            }

            // Restore the original FrameRounding and ItemSpacing values
            style.FrameRounding = originalFrameRounding;
            style.ItemSpacing = originalItemSpacing;
        }

        ImGui::EndMenuBar();
    }
}
