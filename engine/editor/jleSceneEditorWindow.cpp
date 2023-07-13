// Copyright (c) 2023. Johan Lind

#include "jleSceneEditorWindow.h"
#include "editor/jleEditorSceneObjectsWindow.h"
#include "jleEditor.h"
#include "jleInput.h"

#include "ImGui/imgui.h"

#include "glm/common.hpp"
#include "jleFramebufferPicking.h"
#include "jleQuadRendering.h"
#include "jleStaticOpenGLState.h"
#include "jleTexture.h"
#include "jleTransform.h"
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

#include "btBulletDynamicsCommon.h"

#include "jlePhysics.h"
#include <cRigidbody.h>

jleSceneEditorWindow::jleSceneEditorWindow(const std::string &window_name,
                                           std::shared_ptr<jleFramebufferInterface> &framebuffer)
    : iEditorImGuiWindow(window_name)
{
    _framebuffer = framebuffer;

    _pickingFramebuffer = std::make_unique<jleFramebufferPicking>(_framebuffer->width(), _framebuffer->height());

    auto pos = gEditor->editorCamera.getPosition();
    fpvCamController.position = pos;
}

void
jleSceneEditorWindow::update(jleGameEngine &ge)
{
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x,
                      ImGui::GetWindowPos().y,
                      (float)ImGui::GetWindowWidth(),
                      (float)ImGui::GetWindowHeight());

    constexpr float negYOffset = 8;
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

    const auto mouseCoordinateX = getPixelatedMousePosX() + static_cast<int>(jleEditor::editorCamera.getPosition().x);
    const auto mouseCoordinateY = getPixelatedMousePosY() + static_cast<int>(jleEditor::editorCamera.getPosition().y);

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == _lastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == _lastGameWindowHeight)) {
        _lastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        _lastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

        const auto aspect = static_cast<float>(_lastGameWindowHeight) / static_cast<float>(_lastGameWindowWidth);
        auto dims = jleFramebufferInterface::fixedAxisDimensions(
            jleFramebufferInterface::FIXED_AXIS::width, aspect, static_cast<unsigned int>(ImGui::GetWindowHeight()));

        _framebuffer->resize(_lastGameWindowWidth, _lastGameWindowHeight);
    }

    const auto &selectedObject = jleEditorSceneObjectsWindow::GetSelectedObject();

    glBindTexture(GL_TEXTURE_2D, (unsigned int)_framebuffer->texture());
    jleStaticOpenGLState::globalActiveTexture = (unsigned int)_framebuffer->texture();

    // Render the framebuffer as an image
    ImGui::Image((void *)(intptr_t)_framebuffer->texture(),
                 ImVec2(_lastGameWindowWidth, _lastGameWindowHeight),
                 ImVec2(0, 1),
                 ImVec2(1, 0));

    // Picking objects in the scene
    bool canSelectObject = false;
    if (!ImGuizmo::IsOver()) {
        // The mouse is not over any gizmo
        canSelectObject = true;
    }
    if (!selectedObject.lock()) {
        // No object is currently selected
        canSelectObject = true;
    }

    // Note here that the ImGui::Image is the item that is being clicked on!
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && canSelectObject) {

        gCore->rendering().rendering3d().renderMeshesPicking(*_pickingFramebuffer, jleEditor::editorCamera);

        _pickingFramebuffer->bind();

        glFlush();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        unsigned char data[3];
        const int mouseY_flipped = (int)_lastGameWindowHeight - (mouseY - windowPositionY);

        int pixelReadX = (mouseX - windowPositionX) * (_pickingFramebuffer->width() / _lastGameWindowWidth);
        int pixelReadY = mouseY_flipped * (_pickingFramebuffer->height() / _lastGameWindowHeight);
        glReadPixels(pixelReadX, pixelReadY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);

        int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;
        if (pickedID != 0x00ffffff) { // If we did not hit the white background
            LOGI << "Picked object with id: " << pickedID;

            std::vector<std::shared_ptr<jleScene>> scenes;
            if (!gEngine->isGameKilled()) {
                auto &game = ((jleGameEngine *)gCore)->gameRef();
                scenes = game.activeScenesRef();
            }

            scenes.insert(scenes.end(), gEditor->getEditorScenes().begin(), gEditor->getEditorScenes().end());

            for (auto &scene : scenes) {
                for (auto &object : scene->sceneObjects()) {
                    std::shared_ptr<jleObject> o{};
                    object->tryFindChildWithInstanceId(pickedID, o);
                    if (o) {
                        jleEditorSceneObjectsWindow::SetSelectedObject(o);
                    }
                }
            }
        } else {
            LOGI << "Picking object fell out into the universe";
        }

        _pickingFramebuffer->bindDefault();
    }

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

    // Move the Perspective button up slightly
    auto y = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(y - 30 * globalImguiScale);
    auto x = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(x + 8 * globalImguiScale);

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

    ImGui::SameLine();

    ImGui::Text("(%f, %f, %f)", fpvCamController.position.x, fpvCamController.position.y, fpvCamController.position.z);

    ImGui::SameLine();

    if (ImGui::SmallButton("R")) {
        fpvCamController.backToOrigin();
    }

    ImGui::SameLine();

    if(!gEngine->isGameKilled())
    {
        ImGui::Checkbox("Physics Debug", &gEngine->physics().renderDebugEnabled);
    }

    ImGui::SameLine();

    if (gEditor->projectionType == jleCameraProjection::Perspective) {
        ImGui::Text("[%d, %d] (%f)", _framebuffer->width(), _framebuffer->height(), cameraSpeed);
    } else {
        ImGui::Text("[%d, %d - Ortho Zoom: %f] (%f)",
                    _framebuffer->width(),
                    _framebuffer->height(),
                    orthoZoomValue,
                    cameraSpeed);
    }

    const float *viewMatrix = &jleEditor::editorCamera.getViewMatrix()[0][0];
    const float *projectionMatrix = &jleEditor::editorCamera.getProjectionMatrix()[0][0];
    static const auto identityMatrix = glm::mat4{1.f};
    const static float *identityMatrixPtr = &identityMatrix[0][0];
    // ImGuizmo::DrawGrid(viewMatrix, projectionMatrix, identityMatrixPtr, 25.f);

    /*
    // The following commented code is for a camera controller "cube" in the top left corner.
    // It contains a bug, however, and is not really usable at the moment.

    static bool isManipulating = false;
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
    }
    */

    ImGui::SetCursorPosY(ImGui::GetCursorStartPos().y + 5 * globalImguiScale);
    ImGui::SetCursorPosX(ImGui::GetCursorStartPos().x + 5 * globalImguiScale);

    ImGui::BeginGroup();

    if (auto obj = selectedObject.lock()) {
        glm::mat4 worldMatrixBefore = obj->getTransform().getWorldMatrix();
        EditTransform((float *)viewMatrix, (float *)projectionMatrix, (float *)&worldMatrixBefore[0][0], true);
        glm::mat4 transformMatrix = obj->getTransform().getWorldMatrix();
        if (transformMatrix != worldMatrixBefore) {
            if (!gEngine->isGameKilled()) {
                if (auto rb = obj->getComponent<cRigidbody>()) {
                    obj->getTransform().setWorldMatrix(worldMatrixBefore);

                    // Remove scaling from the world matrix (bullet don't want the scaling)
                    glm::vec3 size;
                    size.x = glm::length(glm::vec3(worldMatrixBefore[0])); // Basis vector X
                    size.y = glm::length(glm::vec3(worldMatrixBefore[1])); // Basis vector Y
                    size.z = glm::length(glm::vec3(worldMatrixBefore[2])); // Basis vector Z
                    worldMatrixBefore =
                        glm::scale(worldMatrixBefore, glm::vec3(1.f / size.x, 1.f / size.y, 1.f / size.z));

                    btTransform transform;
                    transform.setFromOpenGLMatrix((btScalar *)&worldMatrixBefore);

                    rb->getBody()->setWorldTransform(transform);

                    rb->getBody()->setLinearVelocity(btVector3{0.f, 0.f, 0.f});
                    rb->getBody()->setAngularVelocity(btVector3{0.f, 0.f, 0.f});
                    rb->getBody()->activate(true);
                }else
                {
                    obj->getTransform().setWorldMatrix(worldMatrixBefore);
                }
            } else {
                obj->getTransform().setWorldMatrix(worldMatrixBefore);
            }
        }
    }

    ImGui::EndGroup();

    // If window is hovered and Gizmo is not being moved/used
    if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing()) {
        auto t = ge.deltaFrameTime();
        auto dragDelta = ImGui::GetMouseDragDelta(1);

        if (jleEditor::projectionType == jleCameraProjection::Perspective || ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
            fpvCamController.applyPerspectiveMouseMovementDelta(glm::vec2{dragDelta.x, dragDelta.y}, t * 100.f);
        } else {
            fpvCamController.move(glm::vec3{dragDelta.x, dragDelta.y, 0.f} * t * 5.f);
        }

        if (ImGui::IsKeyDown(ImGuiKey_W)) {
            if (jleEditor::projectionType == jleCameraProjection::Orthographic) {
                fpvCamController.moveUp(cameraSpeed * t);
            } else {
                fpvCamController.moveForward(cameraSpeed * t);
            }
        }
        if (ImGui::IsKeyDown(ImGuiKey_S)) {
            if (jleEditor::projectionType == jleCameraProjection::Orthographic) {
                fpvCamController.moveDown(cameraSpeed * t);
            } else {
                fpvCamController.moveBackward(cameraSpeed * t);
            }
        }
        if (ImGui::IsKeyDown(ImGuiKey_D)) {
            fpvCamController.moveRight(cameraSpeed * t);
        }
        if (ImGui::IsKeyDown(ImGuiKey_A)) {
            fpvCamController.moveLeft(cameraSpeed * t);
        }
        if (ImGui::IsKeyDown(ImGuiKey_Space)) {
            fpvCamController.moveUp(cameraSpeed * t);
        }
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
            fpvCamController.moveDown(cameraSpeed * t);
        }

        jleEditor::editorCamera.setViewMatrix(fpvCamController.getLookAtViewMatrix(), fpvCamController.position);

        auto currentScroll = gCore->input().mouse->scrollY();
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && currentScroll != 0.f) {
            orthoZoomValue -= currentScroll * 3.f * ge.deltaFrameTime();
            orthoZoomValue = glm::clamp(orthoZoomValue, 0.01f, 2.f);
        } else if(currentScroll != 0.f){
            cameraSpeed -= currentScroll * 2000.f * ge.deltaFrameTime();
            cameraSpeed = glm::clamp(cameraSpeed, 0.2f, 500.f);
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
        if (ImGui::IsKeyPressed(ImGuiKey_T) && !ImGuizmo::IsUsing())
            _currentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R) && !ImGuizmo::IsUsing())
            _currentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_Z) && !ImGuizmo::IsUsing())
            _currentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::IsKeyPressed(ImGuiKey_U) && !ImGuizmo::IsUsing())
            _currentGizmoOperation = ImGuizmo::UNIVERSAL;
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

        if (_currentGizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }

        /*
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
        }*/
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