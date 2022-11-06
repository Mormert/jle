// Copyright (c) 2022. Johan Lind

#include "jleGameEditorWindow.h"
#include "jleWindow.h"
#include <GLFW/glfw3.h>

#include "ImGui/imgui.h"

#include "jleMouseInput.h"
#include "jleStaticOpenGLState.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include "glad/glad.h"

#endif

jleGameEditorWindow::jleGameEditorWindow(const std::string &window_name) : iEditorImGuiWindow{window_name}
{
    gGameEditorWindow = this;
}

void
jleGameEditorWindow::update(jleGameEngine &ge)
{
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    constexpr float negYOffset = 6;
    constexpr float negXOffset = 6;

    const auto &cursorScreenPos = ImGui::GetCursorScreenPos();
    const auto viewport = ImGui::GetMainViewport();
    _windowPositionX = cursorScreenPos.x - viewport->Pos.x;
    _windowPositionY = cursorScreenPos.y - viewport->Pos.y;

    const auto &internalInputMouse = gCore->input().mouse;
    const auto &engineFramebufferMain = gEngine->mainFramebuffer;
    internalInputMouse->setScreenBeginCoords(_windowPositionX, _windowPositionY);
    internalInputMouse->setScreenSize(width(), height());

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == _lastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == _lastGameWindowHeight)) {
        _lastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        _lastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

        gEngine->gameWindowResizedEvent((int)_lastGameWindowWidth, (int)_lastGameWindowHeight);

        /*  auto dims = ge.framebufferDimensions(static_cast<unsigned int>(ImGui::GetWindowWidth()),
                                               static_cast<unsigned int>(ImGui::GetWindowHeight()));
          ge.mainFramebuffer->resize(dims.first, dims.second);
          internalInputMouse->setPixelatedScreenSize(dims.first, dims.second);*/
    }

    // Get the texture from the framebuffer
    glBindTexture(GL_TEXTURE_2D, (unsigned int)ge.mainFramebuffer->texture());
    jleStaticOpenGLState::globalActiveTexture = (unsigned int)ge.mainFramebuffer->texture();
    ImGui::Image((void *)(intptr_t)ge.mainFramebuffer->texture(),
                 ImVec2(_lastGameWindowWidth, _lastGameWindowHeight),
                 ImVec2(0, 1),
                 ImVec2(1, 0));

    if (ImGui::IsWindowFocused() != _wasFocused) {
        _wasFocused = ImGui::IsWindowFocused();
        ge.input().inputEnabled(_wasFocused);
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
