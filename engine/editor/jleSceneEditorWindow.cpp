// Copyright (c) 2022. Johan Lind

#include "jleSceneEditorWindow.h"
#include "editor/jleEditorSceneObjectsWindow.h"
#include "jleEditor.h"

#include "ImGui/imgui.h"

#include "cTransform.h"
#include "glm/common.hpp"
#include "jleFrameBuffer.h"
#include "jleQuadRendering.h"
#include "jleStaticOpenGLState.h"
#include "jleTexture.h"
#include "jleWindow.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include "glad/glad.h"

#endif

#include <GLFW/glfw3.h>

jleSceneEditorWindow::jleSceneEditorWindow(
    const std::string &window_name,
    std::shared_ptr<jleFramebuffer> &framebuffer)
    : iEditorImGuiWindow(window_name),
      _transformMarkerImage("EditorResources/icons/transform_marker.png") {
    _framebuffer = framebuffer;
    _transformMarkerTexture =
        std::make_unique<jleTexture>(_transformMarkerImage);

    _texturedQuad.texture = _transformMarkerTexture;
    _texturedQuad.width = 128;
    _texturedQuad.height = 128;
    _texturedQuad.depth = 1000.f;
    _texturedQuad.textureX = 0;
    _texturedQuad.textureY = 0;
}

void jleSceneEditorWindow::update(jleGameEngine &ge) {
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
    const int32_t windowPositionX =
        int32_t(cursorScreenPos.x) - viewport->Pos.x;
    const int32_t windowPositionY =
        int32_t(cursorScreenPos.y) - viewport->Pos.y;

    const auto previousFrameCursorPos = _lastCursorPos;
    _lastCursorPos = jleCore::core->window->cursor();
    const int32_t mouseX = _lastCursorPos.first;
    const int32_t mouseY = _lastCursorPos.second;
    const int32_t mouseDeltaX = mouseX - previousFrameCursorPos.first;
    const int32_t mouseDeltaY = mouseY - previousFrameCursorPos.second;

    const auto getPixelatedMousePosX = [&]() -> int32_t {
        const float ratio =
            float(_framebuffer->width()) / float(_lastGameWindowWidth);
        return int(ratio * float(mouseX - windowPositionX));
    };

    const auto getPixelatedMousePosY = [&]() -> int32_t {
        const float ratio =
            float(_framebuffer->height()) / float(_lastGameWindowHeight);
        return int(ratio * float(mouseY - windowPositionY));
    };

    const auto mouseCoordinateX =
        getPixelatedMousePosX() + jleEditor::_editorCamera.intX();
    const auto mouseCoordinateY =
        getPixelatedMousePosY() + jleEditor::_editorCamera.intY();

    static float zoomValue = 1.f;

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset ==
              _lastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y -
                  negYOffset ==
              _lastGameWindowHeight)) {
        _lastGameWindowWidth =
            ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        _lastGameWindowHeight = ImGui::GetWindowHeight() -
                                ImGui::GetCursorStartPos().y - negYOffset;

        auto dims = ge.framebufferDimensions(
            static_cast<unsigned int>(ImGui::GetWindowWidth()),
            static_cast<unsigned int>(ImGui::GetWindowHeight()));
        _framebuffer->resize(dims.first * zoomValue, dims.second * zoomValue);
    }

    const auto &selectedObject =
        jleEditorSceneObjectsWindow::GetSelectedObject();
    std::shared_ptr<cTransform> transform{nullptr};

    // Render the transform marker only in the editor window
    if (auto object = selectedObject.lock()) {
        transform = object->component<cTransform>();
        if (transform) {
            _texturedQuad.x = transform->worldX() - 64.f;
            _texturedQuad.y = transform->worldY() - 64.f;
            std::vector<texturedQuad> texturedQuads{_texturedQuad};
            jleCore::core->rendering->quads().render(*_framebuffer,
                                                     jleEditor::_editorCamera,
                                                     texturedQuads,
                                                     {},
                                                     {},
                                                     false);
        }
    }

    glBindTexture(GL_TEXTURE_2D, (unsigned int)_framebuffer->texture());
    jleStaticOpenGLState::globalActiveTexture =
        (unsigned int)_framebuffer->texture();

    // Render the framebuffer as an image
    ImGui::Image((void *)(intptr_t)_framebuffer->texture(),
                 ImVec2(_lastGameWindowWidth, _lastGameWindowHeight),
                 ImVec2(0, 1),
                 ImVec2(1, 0));

    if (ImGui::IsWindowHovered()) {
        auto t = ge.status->deltaFrameTime();
        auto dragDelta = ImGui::GetMouseDragDelta(1);

        jleEditor::_editorCamera._x += dragDelta.x * 10.f * zoomValue * t;
        jleEditor::_editorCamera._y += dragDelta.y * 10.f * zoomValue * t;

        jleEditor::_editorCamera._xNoOffset =
            jleEditor::_editorCamera._x + _framebuffer->width() * .5;
        jleEditor::_editorCamera._yNoOffset =
            jleEditor::_editorCamera._y + _framebuffer->height() * .5;

        jleEditor::_editorCamera.applyPerspectiveMouseMovementDelta(
            glm::vec2{dragDelta.x, -dragDelta.y});

        if (ImGui::IsKeyDown(ImGuiKey_W)) {
            jleEditor::_editorCamera.movePerspectiveForward(
                115.f * ge.status->deltaFrameTime());
        }
        if (ImGui::IsKeyDown(ImGuiKey_S)) {
            jleEditor::_editorCamera.movePerspectiveBackward(
                115.f * ge.status->deltaFrameTime());
        }
        if (ImGui::IsKeyDown(ImGuiKey_D)) {
            jleEditor::_editorCamera.movePerspectiveRight(
                115.f * ge.status->deltaFrameTime());
        }
        if (ImGui::IsKeyDown(ImGuiKey_A)) {
            jleEditor::_editorCamera.movePerspectiveLeft(
                115.f * ge.status->deltaFrameTime());
        }
        if (ImGui::IsKeyDown(ImGuiKey_Space)) {
            jleEditor::_editorCamera.movePerspectiveUp(
                115.f * ge.status->deltaFrameTime());
        }
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
            jleEditor::_editorCamera.movePerspectiveDown(
                115.f * ge.status->deltaFrameTime());
        }

        auto currentScroll = jleCore::core->input->mouse->scrollY();
        if (currentScroll != 0.f) {
            zoomValue -= currentScroll * 0.1f;
            zoomValue = glm::clamp(zoomValue, 0.25f, 5.f);
            auto dims = ge.framebufferDimensions(
                static_cast<unsigned int>(ImGui::GetWindowWidth()),
                static_cast<unsigned int>(ImGui::GetWindowHeight()));

            const auto oldWidth = _framebuffer->width();
            const auto oldHeight = _framebuffer->height();
            const auto widthDiff = dims.first * zoomValue - oldWidth;
            const auto heightDiff = dims.second * zoomValue - oldHeight;
            jleEditor::_editorCamera._x -= widthDiff * .5f;
            jleEditor::_editorCamera._y -= heightDiff * .5f;
            _framebuffer->resize(dims.first * zoomValue,
                                 dims.second * zoomValue);
        }

        static int draggingTransformMarker = 0;
        if (ImGui::IsMouseClicked(0)) {
            if ((mouseCoordinateX >= _texturedQuad.x &&
                 mouseCoordinateX <= _texturedQuad.x + 128) &&
                (mouseCoordinateY >= _texturedQuad.y &&
                 mouseCoordinateY <= _texturedQuad.y + 128)) {
                LOGV << "Inside AABB " << mouseCoordinateX - _texturedQuad.x
                     << ' ' << mouseCoordinateY - _texturedQuad.y;

                constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
                    redPart = {217, 87, 99, 255};
                constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
                    greenPart = {153, 229, 80, 255};
                constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
                    bluePart = {99, 155, 255, 255};

                const auto pixels = _transformMarkerImage.pixelAtLocation(
                    mouseCoordinateX - _texturedQuad.x,
                    mouseCoordinateY - _texturedQuad.y);

                if (pixels == redPart) {
                    draggingTransformMarker = 1;
                }
                else if (pixels == greenPart) {
                    draggingTransformMarker = 2;
                }
                else if (pixels == bluePart) {
                    draggingTransformMarker = 3;
                }
            }
        }

        if (ImGui::IsMouseReleased(0) && draggingTransformMarker) {
            draggingTransformMarker = 0;
        }

        if (ImGui::IsMouseDragging(0)) {
            if (draggingTransformMarker == 1) {
                transform->worldPosition(mouseCoordinateX,
                                         mouseCoordinateY,
                                         transform->worldDepth());
            }
            else if (draggingTransformMarker == 2) {
                transform->worldPositionX(mouseCoordinateX);
            }
            else if (draggingTransformMarker == 3) {
                transform->worldPositionY(mouseCoordinateY);
            }
        }

        if (!draggingTransformMarker && ImGui::IsMouseClicked(0)) {
            // Select closest object from mouse click
            auto &game = ((jleGameEngine *)jleCore::core)->gameRef();
            const auto &scenes = game.activeScenesRef();

            std::unordered_map<std::shared_ptr<cTransform>,
                               std::shared_ptr<jleObject>>
                transformsMap;
            for (auto &scene : scenes) {
                for (auto &object : scene->sceneObjects()) {
                    auto objectsTransform = object->component<cTransform>();
                    if (objectsTransform) {
                        transformsMap.insert(
                            std::make_pair(objectsTransform, object));
                    }
                }
            }

            std::shared_ptr<cTransform> closestTransform = nullptr;
            std::shared_ptr<jleObject> selectedObject = nullptr;
            for (auto &transformPair : transformsMap) {
                const auto &transform = transformPair.first;
                const auto &object = transformPair.second;
                if (closestTransform == nullptr) {
                    closestTransform = transform;
                    selectedObject = object;
                }
                else if ((pow(transform->worldX() - mouseCoordinateX, 2) +
                          pow(transform->worldY() - mouseCoordinateY, 2)) <
                         (pow(closestTransform->worldX() - mouseCoordinateX,
                              2)) +
                             pow(closestTransform->worldY() - mouseCoordinateY,
                                 2)) {
                    closestTransform = transform;
                    selectedObject = object;
                }
            }

            if (selectedObject) {
                jleEditorSceneObjectsWindow::SetSelectedObject(selectedObject);
            }
        }
    }

    ImGui::End();
}
