// Copyright (c) 2022. Johan Lind

#include "SceneEditorWindow.h"
#include "jleEditor.h"

#include "ImGui/imgui.h"

#include "GLState.h"
#include "glm/common.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#endif

jle::SceneEditorWindow::SceneEditorWindow(const std::string &window_name, std::shared_ptr<iFramebuffer> &framebuffer)
        : iEditorImGuiWindow(window_name) {
    mFramebuffer = framebuffer;
}

void jle::SceneEditorWindow::Update(jle::jleGameEngine &ge) {
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    constexpr float negYOffset = 6;
    constexpr float negXOffset = 6;

    static float zoomValue = 1.f;

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == mLastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == mLastGameWindowHeight)) {
        mLastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        mLastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

        auto dims = ge.GetFramebufferDimensions(static_cast<unsigned int>(ImGui::GetWindowWidth()),
                                                static_cast<unsigned int>(ImGui::GetWindowHeight()));
        mFramebuffer->ResizeFramebuffer(dims.first * zoomValue, dims.second * zoomValue);
    }

    // Get the texture from the framebuffer
    glBindTexture(GL_TEXTURE_2D, (unsigned int) mFramebuffer->GetTexture());
    jle::GLState::globalActiveTexture = (unsigned int) mFramebuffer->GetTexture();
    ImGui::Image((void *) (intptr_t) mFramebuffer->GetTexture(), ImVec2(mLastGameWindowWidth, mLastGameWindowHeight),
                 ImVec2(0, 1), ImVec2(1, 0));

    if(ImGui::IsWindowHovered())
    {
        auto dragDelta = ImGui::GetMouseDragDelta(1);
        jleEditor::mEditorCamera.mX += dragDelta.x * 0.001f * zoomValue;
        jleEditor::mEditorCamera.mY += dragDelta.y * 0.001f * zoomValue;

        auto currentScroll = jle::jleCore::core->input->mouse->GetScrollY();
        if(currentScroll != 0.f)
        {
            zoomValue -= currentScroll * 0.1f;
            zoomValue = glm::clamp(zoomValue, 0.25f, 5.f);
            auto dims = ge.GetFramebufferDimensions(static_cast<unsigned int>(ImGui::GetWindowWidth()),
                                                    static_cast<unsigned int>(ImGui::GetWindowHeight()));

            mFramebuffer->ResizeFramebuffer(dims.first * zoomValue, dims.second * zoomValue);
        }
    }

    ImGui::End();

}
