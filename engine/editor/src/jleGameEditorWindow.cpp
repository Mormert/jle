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

#include "game/jleGame.h"
#include "modules/graphics/core/jleFrameBufferInterface.h"
#include "modules/input/hardware/jleMouseInput.h"
#include "modules/input/jleInputModule.h"
#include "modules/windowing/jleWindowModule.h"

#include "modules/graphics/core/jleIncludeGL.h"

#include "jlECS/jlECS.h"
#include "modules/graphics/editor/jleGraphicsModuleEditor.h"
#include "modules/graphics/jleGraphicsModule.h"

#include <ImGui/imgui.h>

jleGameEditorWindow::jleGameEditorWindow(const std::string &window_name) : jleEditorWindowInterface{window_name}
{
}

void
jleGameEditorWindow::renderUI(jleEngineUpdateContext &ctx)
{
    ZoneScoped;
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    if (ctx.gameRuntime.isGameKilled()) {
        ImGuiStyle &style = ImGui::GetStyle();

        const char* startGameString = "   Start Game   ";
        float size = ImGui::CalcTextSize(startGameString).x + style.FramePadding.x * 2.0f;
        float avail = ImGui::GetContentRegionAvail().x;

        float off = (avail - size) * 0.5f;
        if (off > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2.f);

        if (ImGui::Button(startGameString)) {
            ctx.gameRuntime.startGame(ctx.serializationContext);
        }

        ImGui::End();
        return;
    }

    auto inputModule = ctx.gameRuntime.getGame().getModules().getModule<jleInputModule>();
    auto graphicsModule = ctx.gameRuntime.getGame().getModules().getModule<jleGraphicsModuleEditor>();
    if (!inputModule || !graphicsModule) {
        ImGui::Text("Error: Input or Graphics module not found.");
        return;
    }

    constexpr float negYOffset = 8;
    constexpr float negXOffset = 6;

    const auto &cursorScreenPos = ImGui::GetCursorScreenPos();
    const auto viewport = ImGui::GetMainViewport();
    _windowPositionX = cursorScreenPos.x - viewport->Pos.x;
    _windowPositionY = cursorScreenPos.y - viewport->Pos.y;

    auto &internalInputMouse = inputModule->mouse;
    internalInputMouse.setScreenBeginCoords(_windowPositionX, _windowPositionY);
    internalInputMouse.setScreenSize(width(), height());

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == _lastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == _lastGameWindowHeight)) {
        _lastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        _lastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

        graphicsModule->setGameWindowSize((uint32_t)_lastGameWindowHeight, (uint32_t)_lastGameWindowWidth);
    }

    // Get the texture from the framebuffer
    const uint32_t gameWindowTextureId = graphicsModule->getGameWindowTextureId();
    glBindTexture(GL_TEXTURE_2D, gameWindowTextureId);
    ImGui::Image((void *)(intptr_t)gameWindowTextureId,
                 ImVec2(_lastGameWindowWidth, _lastGameWindowHeight),
                 ImVec2(0, 1),
                 ImVec2(1, 0));

    if (ImGui::IsWindowFocused() != _wasFocused) {
        _wasFocused = ImGui::IsWindowFocused();
        inputModule->setInputEnabled(_wasFocused);
    }

    if (ImGui::IsWindowFocused() && inputModule->mouse.isFpsMode() && ImGui::IsKeyPressed(ImGuiKey_Tab)) {
        inputModule->mouse.setFpsMode(false);
    } else if (ImGui::IsKeyPressed(ImGuiKey_Tab)) {
        inputModule->mouse.setFpsMode(true);
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
