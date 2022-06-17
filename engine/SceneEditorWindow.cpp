// Copyright (c) 2022. Johan Lind

#include "SceneEditorWindow.h"
#include "EditorSceneObjectsWindow.h"
#include "jleEditor.h"

#include "ImGui/imgui.h"

#include "GLState.h"
#include "glm/common.hpp"
#include "cTransform.h"
#include "iWindowInternalAPI.h"
#include "iQuadRenderingInternal.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

jle::SceneEditorWindow::SceneEditorWindow(const std::string &window_name, std::shared_ptr<iFramebuffer> &framebuffer)
        : iEditorImGuiWindow(window_name) {
    mFramebuffer = framebuffer;
    mTransformMarkerTexture = jle::jleCore::core->texture_creator->LoadTextureFromPath(
            "EditorResources/icons/transform_marker.png");

    mTexturedQuad.texture = mTransformMarkerTexture;
    mTexturedQuad.width = 128;
    mTexturedQuad.height = 128;
    mTexturedQuad.depth = 1.f;
    mTexturedQuad.textureX = 0;
    mTexturedQuad.textureY = 0;
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

    const auto& cursorScreenPos = ImGui::GetCursorScreenPos();
    const auto viewport = ImGui::GetMainViewport();
    const int32_t windowPositionX = int32_t(cursorScreenPos.x) - viewport->Pos.x;
    const int32_t windowPositionY = int32_t(cursorScreenPos.y) - viewport->Pos.y;

    static float zoomValue = 1.f;

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == mLastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == mLastGameWindowHeight)) {
        mLastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        mLastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

        auto dims = ge.GetFramebufferDimensions(static_cast<unsigned int>(ImGui::GetWindowWidth()),
                                                static_cast<unsigned int>(ImGui::GetWindowHeight()));
        mFramebuffer->ResizeFramebuffer(dims.first * zoomValue, dims.second * zoomValue);
    }

    const auto &selectedObject = EditorSceneObjectsWindow::GetSelectedObject();

    // Render the transform marker only in the editor window
    if (auto object = selectedObject.lock()) {
        if (auto transform = object->GetComponent<cTransform>()) {
            mTexturedQuad.x = transform->x - 64.f;
            mTexturedQuad.y = transform->y - 64.f;
            std::vector<TexturedQuad> texturedQuads {mTexturedQuad};
            auto quadRenderer = ((iQuadRenderingInternal *) (jleCore::core->rendering->quads.get()));
            quadRenderer->Render(*mFramebuffer, jleEditor::mEditorCamera, texturedQuads, false);
        }
    }

    glBindTexture(GL_TEXTURE_2D, (unsigned int) mFramebuffer->GetTexture());
    jle::GLState::globalActiveTexture = (unsigned int) mFramebuffer->GetTexture();

    // Render the framebuffer as an image
    ImGui::Image((void *) (intptr_t) mFramebuffer->GetTexture(), ImVec2(mLastGameWindowWidth, mLastGameWindowHeight),
                 ImVec2(0, 1), ImVec2(1, 0));

    if (ImGui::IsWindowHovered()) {
        auto dragDelta = ImGui::GetMouseDragDelta(1);
        jleEditor::mEditorCamera.mX += dragDelta.x * 0.001f * zoomValue;
        jleEditor::mEditorCamera.mY += dragDelta.y * 0.001f * zoomValue;

        auto currentScroll = jle::jleCore::core->input->mouse->GetScrollY();
        if (currentScroll != 0.f) {
            zoomValue -= currentScroll * 0.1f;
            zoomValue = glm::clamp(zoomValue, 0.25f, 5.f);
            auto dims = ge.GetFramebufferDimensions(static_cast<unsigned int>(ImGui::GetWindowWidth()),
                                                    static_cast<unsigned int>(ImGui::GetWindowHeight()));

            mFramebuffer->ResizeFramebuffer(dims.first * zoomValue, dims.second * zoomValue);
        }

        // Select closest object from mouse click
        if(ImGui::IsMouseClicked(0))
        {
            auto &game = ((jleGameEngine *) jleCore::core)->GetGameRef();
            const auto& scenes = game.GetActiveScenesRef();

            std::unordered_map<std::shared_ptr<cTransform>, std::shared_ptr<jleObject>> transformsMap;
            for(auto& scene : scenes)
            {
                for(auto& object : scene->GetSceneObjects())
                {
                    auto objectsTransform = object->GetComponent<cTransform>();
                    if(objectsTransform)
                    {
                        transformsMap.insert(std::make_pair(objectsTransform, object));
                    }
                }
            }

            const auto& mouseScreenCoordinates =  std::static_pointer_cast<iWindowInternalAPI>(jleCore::core->window)->GetCursor();
            const int mouseX = mouseScreenCoordinates.first;
            const int mouseY = mouseScreenCoordinates.second;

            const auto getPixelatedMousePosX = [&]()-> int32_t {
                const float ratio = float(mFramebuffer->GetWidth()) / float(mLastGameWindowWidth);
                return int(ratio * float(mouseX - windowPositionX));
            };

            const auto getPixelatedMousePosY = [&]()-> int32_t {
                const float ratio = float(mFramebuffer->GetHeight()) / float(mLastGameWindowHeight);
                return int(ratio * float(mouseY - windowPositionY));
            };

            const auto mouseCoordinateX = getPixelatedMousePosX() + jleEditor::mEditorCamera.GetIntX();
            const auto mouseCoordinateY = getPixelatedMousePosY() + jleEditor::mEditorCamera.GetIntY();

            std::shared_ptr<cTransform> closestTransform = nullptr;
            std::shared_ptr<jleObject> selectedObject = nullptr;
            for(auto& transformPair : transformsMap){
                const auto& transform = transformPair.first;
                const auto& object = transformPair.second;
                if(closestTransform == nullptr) {
                    closestTransform = transform;
                    selectedObject = object;
                }
                else if((pow(transform->x - mouseCoordinateX, 2) + pow(transform->y - mouseCoordinateY, 2)) <
                        (pow(closestTransform->x - mouseCoordinateX, 2)) + pow(closestTransform->y - mouseCoordinateY, 2)){
                    closestTransform = transform;
                    selectedObject = object;
                }
            }

            if(selectedObject){
                EditorSceneObjectsWindow::SetSelectedObject(selectedObject);
            }
        }
    }

    ImGui::End();

}
