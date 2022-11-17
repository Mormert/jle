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

jleSceneEditorWindow::jleSceneEditorWindow(const std::string &window_name, std::shared_ptr<jleFramebuffer> &framebuffer)
    : iEditorImGuiWindow(window_name), _transformMarkerImage("EditorResources/icons/transform_marker.png")
{
    _framebuffer = framebuffer;
    _transformMarkerTexture = std::make_unique<jleTexture>(_transformMarkerImage);

    _texturedQuad.texture = _transformMarkerTexture;
    _texturedQuad.width = 128;
    _texturedQuad.height = 128;
    _texturedQuad.depth = 1000.f;
    _texturedQuad.textureX = 0;
    _texturedQuad.textureY = 0;

    _fpvCamController.position = {0.f, 0.f, 250.f};
}

void
jleSceneEditorWindow::update(jleGameEngine &ge)
{
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x,
                      ImGui::GetWindowPos().y,
                      (float)ImGui::GetWindowWidth(),
                      (float)ImGui::GetWindowHeight());

    constexpr float negYOffset = 6;
    constexpr float negXOffset = 6;

    const auto &cursorScreenPos = ImGui::GetCursorScreenPos();
    const auto viewport = ImGui::GetMainViewport();
    const int32_t windowPositionX = int32_t(cursorScreenPos.x) - viewport->Pos.x;
    const int32_t windowPositionY = int32_t(cursorScreenPos.y) - viewport->Pos.y;

    const auto previousFrameCursorPos = _lastCursorPos;
    _lastCursorPos = gCore->window().cursor();
    const int32_t mouseX = _lastCursorPos.first;
    const int32_t mouseY = _lastCursorPos.second;
    const int32_t mouseDeltaX = mouseX - previousFrameCursorPos.first;
    const int32_t mouseDeltaY = mouseY - previousFrameCursorPos.second;

    const auto getPixelatedMousePosX = [&]() -> int32_t {
        const float ratio = float(_framebuffer->width()) / float(_lastGameWindowWidth);
        return int(ratio * float(mouseX - windowPositionX));
    };

    const auto getPixelatedMousePosY = [&]() -> int32_t {
        const float ratio = float(_framebuffer->height()) / float(_lastGameWindowHeight);
        return int(ratio * float(mouseY - windowPositionY));
    };

    const auto mouseCoordinateX =
        getPixelatedMousePosX() + static_cast<int>(jleEditor::editorCamera.getViewPosition().x);
    const auto mouseCoordinateY =
        getPixelatedMousePosY() + static_cast<int>(jleEditor::editorCamera.getViewPosition().y);

    static float zoomValue = 1.f;

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == _lastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == _lastGameWindowHeight)) {
        _lastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        _lastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

        const auto aspect = static_cast<float>(_lastGameWindowHeight) / static_cast<float>(_lastGameWindowWidth);
        auto dims = jleFramebuffer::fixedAxisDimensions(
            jleFramebuffer::FIXED_AXIS::width, aspect, static_cast<unsigned int>(ImGui::GetWindowHeight()));

        _framebuffer->resize(dims.x * zoomValue, dims.y * zoomValue);
    }

    const auto &selectedObject = jleEditorSceneObjectsWindow::GetSelectedObject();
    std::shared_ptr<cTransform> transform{nullptr};

    // Render the transform marker only in the editor window
    if (auto object = selectedObject.lock()) {
        transform = object->component<cTransform>();
        if (transform) {
            _texturedQuad.x = transform->getWorldPosition().x - 64.f;
            _texturedQuad.y = transform->getWorldPosition().y - 64.f;
            _texturedQuad.depth = -10000.f;
            std::vector<texturedQuad> texturedQuads{_texturedQuad};
            gCore->quadRendering().render(*_framebuffer, jleEditor::editorCamera, texturedQuads, {}, {});
        }
    }

    glBindTexture(GL_TEXTURE_2D, (unsigned int)_framebuffer->texture());
    jleStaticOpenGLState::globalActiveTexture = (unsigned int)_framebuffer->texture();

    // Render the framebuffer as an image
    ImGui::Image((void *)(intptr_t)_framebuffer->texture(),
                 ImVec2(_lastGameWindowWidth, _lastGameWindowHeight),
                 ImVec2(0, 1),
                 ImVec2(1, 0));

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

    // Move the Perspective button up slightly
    auto y = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(y - 25 * globalImguiScale);
    auto x = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(x + 5 * globalImguiScale);

    if (jleEditor::projectionType == jleCameraProjection::Orthographic) {
        if (ImGui::Button("Orthographic")) {
            jleEditor::projectionType = jleCameraProjection::Perspective;
            ImGuizmo::SetOrthographic(false);
        }
    } else {
        if (ImGui::Button("Perspective")) {
            jleEditor::projectionType = jleCameraProjection::Orthographic;
            ImGuizmo::SetOrthographic(true);
        }
    }

    const float *viewMatrix = &jleEditor::editorCamera.getViewMatrix()[0][0];
    const float *projectionMatrix = &jleEditor::editorCamera.getProjectionMatrix()[0][0];
    static const auto identityMatrix = glm::mat4{1.f};
    const static float *identityMatrixPtr = &identityMatrix[0][0];
    ImGuizmo::DrawGrid(viewMatrix, projectionMatrix, identityMatrixPtr, 25.f);

    // The following commented code is for a camera controller "cube" in the top left corner.
    // It contains a bug, however, and is not really usable at the moment.

    /*static bool isManipulating = false;
    static glm::mat4 manipulatingMatrix{1.f};
    static glm::mat4 lastFrameMatrix{1.f};

    if (!isManipulating) {
        manipulatingMatrix = jleEditor::editorCamera.getViewMatrix();
    }

    ImGuizmo::ViewManipulate(
        &manipulatingMatrix[0][0],
        250.f,
        ImVec2(ImGui::GetWindowPos().x + 20 * globalImguiScale, ImGui::GetWindowPos().y + 35 * globalImguiScale),
        ImVec2(128 * globalImguiScale, 128 * globalImguiScale),
        0x10101010);

    if (manipulatingMatrix != jleEditor::editorCamera.getViewMatrix()) {
        // The view manipulate function modified the view matrix
        isManipulating = true;
    }

    if (isManipulating) {
        if (lastFrameMatrix == manipulatingMatrix) {
            isManipulating = false;
        }
    }

    if (isManipulating) {
        _fpvCamController.recalculateVectorsFromViewMatrix(manipulatingMatrix);
        lastFrameMatrix = manipulatingMatrix;
    }*/

    ImGui::SetCursorPosY(y - 250 * globalImguiScale);

    if (transform) {
        glm::mat4 worldMatrixBefore = transform->getWorldMatrix();
        EditTransform((float *)viewMatrix, (float *)projectionMatrix, (float *)&worldMatrixBefore[0][0], true);
        glm::mat4 transformMatrix = transform->getWorldMatrix();
        if (transformMatrix != worldMatrixBefore) {
            transform->setWorldMatrix(worldMatrixBefore);
        }
    }

    ImGui::SameLine();

    ImGui::Text(
        "(%f, %f, %f)", _fpvCamController.position.x, _fpvCamController.position.y, _fpvCamController.position.z);

    ImGui::SameLine();

    if (ImGui::SmallButton("R")) {
        _fpvCamController.backToOrigin();
    }

    ImGui::SameLine();

    ImGui::Text("[%d, %d]", _framebuffer->width(), _framebuffer->height());

    // If window is hovered and Gizmo is not being moved/used
    if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing()) {
        auto t = ge.deltaFrameTime();
        auto dragDelta = ImGui::GetMouseDragDelta(1);

        if (jleEditor::projectionType == jleCameraProjection::Perspective || ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
            _fpvCamController.applyPerspectiveMouseMovementDelta(glm::vec2{dragDelta.x, dragDelta.y}, t * 100.f);
        } else {
            _fpvCamController.move(glm::vec3{dragDelta.x, dragDelta.y, 0.f} * t * 5.f);
        }

        constexpr float moveSpeed = 350.f;
        if (ImGui::IsKeyDown(ImGuiKey_W)) {
            if (jleEditor::projectionType == jleCameraProjection::Orthographic) {
                _fpvCamController.moveUp(moveSpeed * t);
            } else {
                _fpvCamController.moveForward(moveSpeed * t);
            }
        }
        if (ImGui::IsKeyDown(ImGuiKey_S)) {
            if (jleEditor::projectionType == jleCameraProjection::Orthographic) {
                _fpvCamController.moveDown(moveSpeed * t);
            } else {
                _fpvCamController.moveBackward(moveSpeed * t);
            }
        }
        if (ImGui::IsKeyDown(ImGuiKey_D)) {
            _fpvCamController.moveRight(moveSpeed * t);
        }
        if (ImGui::IsKeyDown(ImGuiKey_A)) {
            _fpvCamController.moveLeft(moveSpeed * t);
        }
        if (ImGui::IsKeyDown(ImGuiKey_Space)) {
            _fpvCamController.moveUp(moveSpeed * t);
        }
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
            _fpvCamController.moveDown(moveSpeed * t);
        }

        jleEditor::editorCamera.setViewMatrix(_fpvCamController.getLookAtViewMatrix());

        auto currentScroll = gCore->input().mouse->scrollY();
        if (currentScroll != 0.f) {
            zoomValue -= currentScroll * 0.1f;
            zoomValue = glm::clamp(zoomValue, 0.25f, 5.f);

            const auto aspect = static_cast<float>(_lastGameWindowHeight) / static_cast<float>(_lastGameWindowWidth);
            auto dims = jleFramebuffer::fixedAxisDimensions(
                jleFramebuffer::FIXED_AXIS::width, aspect, static_cast<unsigned int>(ImGui::GetWindowHeight()));

            _framebuffer->resize(dims.x * zoomValue, dims.y * zoomValue);
        }

        static int draggingTransformMarker = 0;
        if (ImGui::IsMouseClicked(0)) {
            if ((mouseCoordinateX >= _texturedQuad.x && mouseCoordinateX <= _texturedQuad.x + 128) &&
                (mouseCoordinateY >= _texturedQuad.y && mouseCoordinateY <= _texturedQuad.y + 128)) {
                LOGV << "Inside AABB " << mouseCoordinateX - _texturedQuad.x << ' '
                     << mouseCoordinateY - _texturedQuad.y;

                constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> redPart = {217, 87, 99, 255};
                constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> greenPart = {153, 229, 80, 255};
                constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> bluePart = {99, 155, 255, 255};

                const auto pixels = _transformMarkerImage.pixelAtLocation(mouseCoordinateX - _texturedQuad.x,
                                                                          mouseCoordinateY - _texturedQuad.y);

                if (pixels == redPart) {
                    draggingTransformMarker = 1;
                } else if (pixels == greenPart) {
                    draggingTransformMarker = 2;
                } else if (pixels == bluePart) {
                    draggingTransformMarker = 3;
                }
            }
        }

        if (ImGui::IsMouseReleased(0) && draggingTransformMarker) {
            draggingTransformMarker = 0;
        }

        if (transform && ImGui::IsMouseDragging(0)) {
            if (draggingTransformMarker == 1) {
                transform->setWorldPosition({mouseCoordinateX, mouseCoordinateY, transform->getWorldPosition().z});
            } else if (draggingTransformMarker == 2) {
                const auto t = transform->getWorldPosition();
                transform->setWorldPosition({mouseCoordinateX, t.y, t.z});
            } else if (draggingTransformMarker == 3) {
                const auto t = transform->getWorldPosition();
                transform->setWorldPosition({t.x, mouseCoordinateY, t.z});
            }
        }

        if (!draggingTransformMarker && ImGui::IsMouseClicked(0)) {
            // Select closest object from mouse click
            auto &game = ((jleGameEngine *)gCore)->gameRef();
            const auto &scenes = game.activeScenesRef();

            std::unordered_map<std::shared_ptr<cTransform>, std::shared_ptr<jleObject>> transformsMap;
            for (auto &scene : scenes) {
                for (auto &object : scene->sceneObjects()) {
                    auto objectsTransform = object->component<cTransform>();
                    if (objectsTransform) {
                        transformsMap.insert(std::make_pair(objectsTransform, object));
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
                } else if ((pow(transform->getWorldPosition().x - mouseCoordinateX, 2) +
                            pow(transform->getWorldPosition().y - mouseCoordinateY, 2)) <
                           (pow(closestTransform->getWorldPosition().x - mouseCoordinateX, 2)) +
                               pow(closestTransform->getWorldPosition().y - mouseCoordinateY, 2)) {
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
void
jleSceneEditorWindow::EditTransform(float *cameraView,
                                    float *cameraProjection,
                                    float *matrix,
                                    bool editTransformDecomposition)
{

    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = {1.f, 1.f, 1.f};
    static float bounds[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
    static float boundsSnap[] = {0.1f, 0.1f, 0.1f};
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    if (editTransformDecomposition) {
        if (ImGui::IsKeyPressed(ImGuiKey_T))
            _currentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            _currentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            _currentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", _currentGizmoOperation == ImGuizmo::TRANSLATE))
            _currentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", _currentGizmoOperation == ImGuizmo::ROTATE))
            _currentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", _currentGizmoOperation == ImGuizmo::SCALE))
            _currentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Universal", _currentGizmoOperation == ImGuizmo::UNIVERSAL))
            _currentGizmoOperation = ImGuizmo::UNIVERSAL;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

        if (_currentGizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_S))
            useSnap = !useSnap;
        ImGui::Checkbox("##UseSnap", &useSnap);
        ImGui::SameLine();

        switch (_currentGizmoOperation) {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
        }
        ImGui::Checkbox("Bound Sizing", &boundSizing);
        if (boundSizing) {
            ImGui::PushID(3);
            ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", boundsSnap);
            ImGui::PopID();
        }
    }

    ImGuizmo::Manipulate(cameraView,
                         cameraProjection,
                         _currentGizmoOperation,
                         mCurrentGizmoMode,
                         matrix,
                         NULL,
                         useSnap ? &snap[0] : NULL,
                         boundSizing ? bounds : NULL,
                         boundSizingSnap ? boundsSnap : NULL);
}
