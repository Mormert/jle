// Copyright (c) 2022. Johan Lind

#include "jleGameEditorWindow.h"

#include "3rdparty/ImGui/imgui.h"

#include "jleMouseInputInternal.h"
#include "jleStaticOpenGLState.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include "3rdparty/glad/glad.h"

#endif

jleGameEditorWindow::jleGameEditorWindow(const std::string& window_name)
    : iEditorImGuiWindow{window_name} {
    gGameEditorWindow = this;
}

void jleGameEditorWindow::Update(jleGameEngine& ge) {
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    constexpr float negYOffset = 6;
    constexpr float negXOffset = 6;

    const auto& cursorScreenPos = ImGui::GetCursorScreenPos();
    const auto viewport = ImGui::GetMainViewport();
    mWindowPositionX = cursorScreenPos.x - viewport->Pos.x;
    mWindowPositionY = cursorScreenPos.y - viewport->Pos.y;

    const auto& internalInputMouse =
        std::static_pointer_cast<jleMouseInputInternal>(
            jleCore::core->input->mouse);
    const auto& engineFramebufferMain =
        jleGameEngine::gEngine->framebuffer_main;
    internalInputMouse->SetScreenBeginCoords(mWindowPositionX,
                                             mWindowPositionY);
    internalInputMouse->SetScreenSize(GetWindowWidth(), GetWindowHeight());

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset ==
              mLastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y -
                  negYOffset ==
              mLastGameWindowHeight)) {
        mLastGameWindowWidth =
            ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        mLastGameWindowHeight = ImGui::GetWindowHeight() -
                                ImGui::GetCursorStartPos().y - negYOffset;

        auto dims = ge.GetFramebufferDimensions(
            static_cast<unsigned int>(ImGui::GetWindowWidth()),
            static_cast<unsigned int>(ImGui::GetWindowHeight()));
        ge.framebuffer_main->ResizeFramebuffer(dims.first, dims.second);
        internalInputMouse->SetPixelatedScreenSize(dims.first, dims.second);

        auto& game = ((jleGameEngine *)jleCore::core)->GetGameRef();
        game.mMainCamera.mCameraWidth = dims.first;
        game.mMainCamera.mCameraHeight = dims.second;
    }

    // Get the texture from the framebuffer
    glBindTexture(GL_TEXTURE_2D,
                  (unsigned int)ge.framebuffer_main->GetTexture());
    jleStaticOpenGLState::globalActiveTexture =
        (unsigned int)ge.framebuffer_main->GetTexture();
    ImGui::Image((void *)(intptr_t)ge.framebuffer_main->GetTexture(),
                 ImVec2(mLastGameWindowWidth, mLastGameWindowHeight),
                 ImVec2(0, 1),
                 ImVec2(1, 0));

    if (ImGui::IsWindowFocused() != mWasFocused) {
        mWasFocused = ImGui::IsWindowFocused();
        ge.input->SetInputEnabled(mWasFocused);
    }

    ImGui::End();
}

int jleGameEditorWindow::GetWindowWidth() const {
    return int(mLastGameWindowWidth);
}

int jleGameEditorWindow::GetWindowHeight() const {
    return int(mLastGameWindowHeight);
}

int jleGameEditorWindow::GetWindowPosX() const { return mWindowPositionX; }

int jleGameEditorWindow::GetWindowPosY() const { return mWindowPositionY; }
