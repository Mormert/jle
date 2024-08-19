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

#include "jleGameEditorWindow.h"
#include "modules/graphics/core/jleFrameBufferInterface.h"
#include "modules/input/hardware/jleMouseInput.h"
#include "modules/input/jleInput.h"
#include "modules/windowing/jleWindow.h"

#include "modules/graphics/core/jleIncludeGL.h"

#include <ImGui/imgui.h>


jleGameEditorWindow::jleGameEditorWindow(const std::string &window_name) : jleEditorWindowInterface{window_name}
{
}

void
jleGameEditorWindow::renderUI(jleGameEngine &ge, jleInput& input)
{
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    if (ge.isGameKilled()) {
        ImGuiStyle &style = ImGui::GetStyle();

        float size = ImGui::CalcTextSize("   Start Game   ").x + style.FramePadding.x * 2.0f;
        float avail = ImGui::GetContentRegionAvail().x;

        float off = (avail - size) * 0.5f;
        if (off > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2.f);

        if (ImGui::Button("   Start Game   ")) {
            ge.startGame();
        }

        ImGui::End();
        return;
    }

    constexpr float negYOffset = 8;
    constexpr float negXOffset = 6;

    const auto &cursorScreenPos = ImGui::GetCursorScreenPos();
    const auto viewport = ImGui::GetMainViewport();
    _windowPositionX = cursorScreenPos.x - viewport->Pos.x;
    _windowPositionY = cursorScreenPos.y - viewport->Pos.y;

    const auto &internalInputMouse = input.mouse;
    const auto &engineFramebufferMain = ge.mainScreenFramebuffer;
    internalInputMouse->setScreenBeginCoords(_windowPositionX, _windowPositionY);
    internalInputMouse->setScreenSize(width(), height());

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == _lastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == _lastGameWindowHeight)) {
        _lastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        _lastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

        ge.gameWindowResizedEvent((int)_lastGameWindowWidth, (int)_lastGameWindowHeight);
    }

    // Get the texture from the framebuffer
    glBindTexture(GL_TEXTURE_2D, (unsigned int)ge.mainScreenFramebuffer->texture());
    ImGui::Image((void *)(intptr_t)ge.mainScreenFramebuffer->texture(),
                 ImVec2(_lastGameWindowWidth, _lastGameWindowHeight),
                 ImVec2(0, 1),
                 ImVec2(1, 0));

    if (ImGui::IsWindowFocused() != _wasFocused) {
        _wasFocused = ImGui::IsWindowFocused();
        ge.input().setInputEnabled(_wasFocused);
    }

    if (ImGui::IsWindowFocused() && ge.input().mouse->isFpsMode() && ImGui::IsKeyPressed(ImGuiKey_Tab)) {
        ge.input().mouse->setFpsMode(false);
    } else if (ImGui::IsKeyPressed(ImGuiKey_Tab)) {
        ge.input().mouse->setFpsMode(true);
    }

    ImGui::End();
}

int
jleGameEditorWindow::width() const
{
    return int(_lastGameWindowWidth);
}

int
jleGameEditorWindow::height() const
{
    return int(_lastGameWindowHeight);
}

int
jleGameEditorWindow::x() const
{
    return _windowPositionX;
}

int
jleGameEditorWindow::y() const
{
    return _windowPositionY;
}
