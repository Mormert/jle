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

#include "jleSceneEditorWindow.h"
#include "jleEditor.h"
#include "jleEditorGizmos.h"
#include "jleEditorSceneObjectsWindow.h"
#include "game/jleGame.h"
#include "modules/graphics/core/jleFramebufferMultisample.h"
#include "modules/graphics/core/jleFramebufferPicking.h"
#include "modules/graphics/core/jleIncludeGL.h"
#include "modules/graphics/jle3DSettings.h"
#include "modules/graphics/jleFramePacket.h"
#include "modules/graphics/jleGraphics.h"
#include "modules/graphics/jleTexture.h"
#include "modules/graphics/runtime/components/cMesh.h"
#include "modules/input/jleInput.h"
#include "modules/physics/components/cRigidbody.h"
#include "modules/physics/jlePhysics.h"
#include "modules/windowing/jleWindow.h"
#include "modules/core/components/cTransform.h"
#include <modules/graphics/core/jleFramebufferScreen.h>

#include <ImGui/imgui.h>
#include <btBulletDynamicsCommon.h>
#include <glm/common.hpp>

jleSceneEditorWindow::jleSceneEditorWindow(const std::string &window_name) : jleEditorWindowInterface(window_name)
{
    constexpr int initialX = 1024, initialY = 1024;
    _framebuffer = std::make_shared<jleFramebufferScreen>(initialX, initialY);

    _pickingFramebuffer = std::make_unique<jleFramebufferPicking>(_framebuffer->width(), _framebuffer->height());
    _msaa = std::make_unique<jleFramebufferMultisample>(_framebuffer->width(), _framebuffer->height(), 4);
}

void
jleSceneEditorWindow::renderUI(const RenderUIInput& input)
{
    if (!isOpened) {
        return;
    }

    auto& editorUpdate = input.editorUpdate;
    auto& selectedObjects = *input.selectedObjects;
    auto& ecs = input.ecs;

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
    const int32_t windowPositionX = int32_t(cursorScreenPos.x) - (int32_t)viewport->Pos.x;
    const int32_t windowPositionY = int32_t(cursorScreenPos.y) - (int32_t)viewport->Pos.y;

    const auto previousFrameCursorPos = _lastCursorPos;
    _lastCursorPos = editorUpdate.engineUpdateContext.windowModule.cursor();
    const int32_t mouseX = _lastCursorPos.first;
    const int32_t mouseY = _lastCursorPos.second;
    const int32_t mouseDeltaX = mouseX - previousFrameCursorPos.first;
    const int32_t mouseDeltaY = mouseY - previousFrameCursorPos.second;

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == _lastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == _lastGameWindowHeight))
    {
        _lastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        _lastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

        _framebuffer->resize(_lastGameWindowWidth, _lastGameWindowHeight);
    }

    glBindTexture(GL_TEXTURE_2D, (unsigned int)_framebuffer->texture());

    ImGui::Image((void *)(intptr_t)_framebuffer->texture(),
                 ImVec2(_lastGameWindowWidth, _lastGameWindowHeight),
                 ImVec2(0, 1),
                 ImVec2(1, 0));

    bool canSelectObject = true;
    if (ImGuizmo::IsOver()) {
        canSelectObject = false;
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && canSelectObject) {
        input.editorUpdate.engineUpdateContext.rendererModule.renderMeshesPicking(
            *_pickingFramebuffer, _renderCamera, input.editorUpdate.engineUpdateContext.currentFramePacket);

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
        if (pickedID != 0x00ffffff) {
            LOGI << "Picked object with id: " << pickedID;
            assert(false); // Todo
        } else {
            LOGI << "Picking missed or background.";
        }

        _pickingFramebuffer->bindDefault();
    }

    {
        auto y = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(y - 30 * globalImguiScale);
        auto x = ImGui::GetCursorPosX();
        ImGui::SetCursorPosX(x + 8 * globalImguiScale);

        if (_perspectiveCamera) {
            if (ImGui::Button("Orthographic")) {
                _perspectiveCamera = false;
                ImGuizmo::SetOrthographic(false);
            }
        } else {
            if (ImGui::Button("Perspective")) {
                _perspectiveCamera = true;
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
        if (!input.editorUpdate.engineUpdateContext.gameRuntime.isGameKilled()) {
            bool physicsEnabled = false;
            ImGui::Checkbox("Physics Debug", &physicsEnabled);
            // This snippet just stubs the variable; you'd implement it in your renderer or physics system
            assert(!physicsEnabled);
        }

        ImGui::SameLine();
        if (_renderCamera.getProjectionType() == jleCameraProjection::Perspective) {
            ImGui::Text("[%d, %d] (%f)", _framebuffer->width(), _framebuffer->height(), cameraSpeed);
        } else {
            ImGui::Text("[%d, %d - Ortho Zoom: %f] (%f)",
                        _framebuffer->width(),
                        _framebuffer->height(),
                        orthoZoomValue,
                        cameraSpeed);
        }
    }

    const float *viewMatrix = &_renderCamera.getViewMatrix()[0][0];
    const float *projectionMatrix = &_renderCamera.getProjectionMatrix()[0][0];
    static const auto identityMatrix = glm::mat4{1.f};
    const static float *identityMatrixPtr = &identityMatrix[0][0];

    // SHIFT, WASD, camera controls, etc. (unchanged)
    if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing())
    {
        auto t = input.editorUpdate.engineUpdateContext.frameInfo.getDeltaTime();
        auto dragDelta = ImGui::GetMouseDragDelta(1);

        if (_renderCamera.getProjectionType() == jleCameraProjection::Perspective ||
            ImGui::IsKeyDown(ImGuiKey_LeftShift))
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                fpvCamController.applyPerspectiveMouseMovementDelta(glm::vec2{mouseDeltaX, mouseDeltaY}, 300.f);
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }
        }
        else
        {
            fpvCamController.move(glm::vec3{dragDelta.x, dragDelta.y, 0.f} * t * 5.f);
        }

        if (ImGui::IsKeyDown(ImGuiKey_W)) {
            if (_renderCamera.getProjectionType() == jleCameraProjection::Orthographic) {
                fpvCamController.moveUp(cameraSpeed * t);
            } else {
                fpvCamController.moveForward(cameraSpeed * t);
            }
        }
        if (ImGui::IsKeyDown(ImGuiKey_S)) {
            if (_renderCamera.getProjectionType() == jleCameraProjection::Orthographic) {
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

        if (dragDelta.x != 0 || dragDelta.y != 0) {
            _renderCamera.setViewMatrix(fpvCamController.getLookAtViewMatrix(), fpvCamController.position);
        }

        auto currentScroll = input.editorUpdate.engineUpdateContext.inputModule.mouse.scrollY();
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && currentScroll != 0.f) {
            orthoZoomValue -= currentScroll * 1.f * t;
            orthoZoomValue = glm::clamp(orthoZoomValue, 0.01f, 2.f);
        } else if (currentScroll != 0.f) {
            cameraSpeed += currentScroll * 200.f * t;
            cameraSpeed = glm::clamp(cameraSpeed, 0.2f, 500.f);
        }
    }

    ImGui::SetCursorPosY(ImGui::GetCursorStartPos().y + 5 * globalImguiScale);
    ImGui::SetCursorPosX(ImGui::GetCursorStartPos().x + 5 * globalImguiScale);

    ImGui::BeginGroup();
    {
        if (selectedObjects.size() > 1)
        {
            std::vector<std::pair<cTransform*, int>> transforms;
            transforms.reserve(selectedObjects.size());
            for (auto& objRef : selectedObjects)
            {
                if (objRef.isValid()) {
                    auto* t = objRef.getComponentPtr<cTransform>();
                    if (t) {
                        transforms.push_back(std::make_pair(t, objRef.objectIndex()));
                    }
                }
            }

            if (!transforms.empty())
            {
                glm::vec3 avgPos(0.f);
                for (auto [transform, objectIndex] : transforms) {
                    avgPos += transform->getPosition();
                }
                avgPos /= (float)transforms.size();

                glm::quat  baseRot   = transforms.front().first->getRotation();
                glm::vec3  baseScale = transforms.front().first->getScale();

                glm::mat4 centerMatrix = glm::translate(glm::mat4(1.f), avgPos)
                                         * glm::mat4_cast(baseRot)
                                         * glm::scale(glm::mat4(1.f), baseScale);

                if (!ImGuizmo::IsUsing() && !_multiGizmoIsActive) {
                    _multiGizmoCurrentMatrix = centerMatrix;
                }

                // If user starts dragging the gizmo this frame
                if (!_multiGizmoIsActive && ImGuizmo::IsOver() && ImGuizmo::IsUsing()) {
                    _multiGizmoIsActive = true;
                    _multiGizmoInitialMatrix = _multiGizmoCurrentMatrix;
                }

                EditTransform((float*)viewMatrix,
                              (float*)projectionMatrix,
                              (float*)&_multiGizmoCurrentMatrix[0][0],
                              true);

                if (_multiGizmoIsActive && !ImGuizmo::IsUsing())
                {
                    _multiGizmoIsActive = false;

                    glm::mat4 delta = _multiGizmoCurrentMatrix * glm::inverse(_multiGizmoInitialMatrix);

                    for (auto [transform, objectIndex] : transforms)
                    {
                        glm::mat4 oldWorld = transform->getWorldMatrix();
                        glm::mat4 newWorld = delta * oldWorld;
                        transform->setWorldMatrix(newWorld);

                        auto objectRef = ecs.getObject(objectIndex);
                        if (auto* rb = objectRef.getComponentPtr<cRigidbody>())
                        {
                            if (auto* meshComp = objectRef.getComponentPtr<cMesh>()) {
                                rb->setWorldMatrixAndScaleRigidbody(&input.physics, *transform, *meshComp);
                            }
                        }
                    }
                }
            }
        }
        else if (selectedObjects.size() == 1)
        {
            auto& selObj = selectedObjects.front();
            if (selObj.isValid()) {
                auto* transform = selObj.getComponentPtr<cTransform>();
                auto* meshComponent = selObj.getComponentPtr<cMesh>();
                if(transform)
                {
                    glm::mat4 worldMatrixBefore = transform->getWorldMatrix();
                    EditTransform((float *)viewMatrix,
                                  (float *)projectionMatrix,
                                  (float *)&worldMatrixBefore[0][0],
                                  true);

                    // If user changed it
                    glm::mat4 transformMatrix = transform->getWorldMatrix();
                    if (transformMatrix != worldMatrixBefore) {
                        if (!editorUpdate.engineUpdateContext.gameRuntime.isGameKilled()) {
                            if (auto *rb = selObj.getComponentPtr<cRigidbody>()) {
                                if(meshComponent){
                                    rb->setWorldMatrixAndScaleRigidbody(
                                        &input.physics,
                                        *transform,
                                        *meshComponent);
                                }
                            } else {
                                transform->setWorldMatrix(worldMatrixBefore);
                            }
                        } else {
                            transform->setWorldMatrix(worldMatrixBefore);
                        }
                    }

                    // Draw highlight
                    if (meshComponent) {
                        if (auto mesh = meshComponent->getMesh()) {
                            glm::mat4 modelMatrix = transform->getWorldMatrix();
                            glm::mat4 matrix1 = glm::scale(modelMatrix, glm::vec3{1.00514159265f});
                            glm::mat4 matrix2 = glm::scale(modelMatrix, glm::vec3{0.99514159265f});
                            auto material = editorUpdate.gizmos.selectedObjectMaterial();

                            editorUpdate.engineUpdateContext.currentFramePacket.sendMesh(
                                mesh, material, matrix1, selObj.objectIndex(), false);
                            editorUpdate.engineUpdateContext.currentFramePacket.sendMesh(
                                mesh, material, matrix2, selObj.objectIndex(), false);
                        }
                    }
                }
            }
        }
    }
    ImGui::EndGroup();

    ImGui::End();
}

void
jleSceneEditorWindow::render(jleFramePacket &framePacket, const jleEditorUpdateContext &ctx)
{
    if (_perspectiveCamera) {
        _renderCamera.setPerspectiveProjection(45.f, _framebuffer->width(), _framebuffer->height(), 10000.f, 0.1f);
    } else {
        _renderCamera.setOrthographicProjection(
            _framebuffer->width() * orthoZoomValue, _framebuffer->height() * orthoZoomValue, 10000.f, -10000.f);
    }

    if (_framebuffer->width() != _msaa->width() || _framebuffer->height() != _msaa->height()) {
        _msaa->resize(_framebuffer->width(), _framebuffer->height());
    }

    framePacket.camera = _renderCamera;
    ctx.engineUpdateContext.rendererModule.render(*_msaa, framePacket);

    _msaa->blitToOther(*_framebuffer);
}

void
jleSceneEditorWindow::renderEditorGrid(jleFramePacket &framePacket)
{
    JLE_SCOPE_PROFILE_CPU(renderEditorGrid)

    jle3DLineVertex v1{glm::vec3{0.f}, glm::vec3{1.f, 0.f, 0.f}, {1.0f, 0.44f, 1.2f}};
    jle3DLineVertex v2 = v1;
    v2.position = glm::vec3{100.f, 0.f, 0.f};
    v1.position = glm::vec3{-100.f, 0.f, 0.f};

    framePacket.sendLine(v1, v2);

    v2.position = glm::vec3{0.f, 100.f, 0.f};
    v1.position = glm::vec3{0.f, -100.f, 0.f};
    v1.color = glm::vec3{0.f, 1.f, 0.f};
    v2.color = glm::vec3{0.f, 1.f, 0.f};
    framePacket.sendLine(v1, v2);

    v2.position = glm::vec3{0.f, 0.f, 100.f};
    v1.position = glm::vec3{0.f, 0.f, -100.f};
    v1.color = glm::vec3{0.f, 0.f, 1.f};
    v2.color = glm::vec3{0.f, 0.f, 1.f};
    framePacket.sendLine(v1, v2);

    v1.color = glm::vec3(1.0f, 0.3f, 0.3f);
    v2.color = glm::vec3(1.0f, 0.3f, 0.3f);
    auto pos = _renderCamera.getPosition();

    float scale = 1.f;
    if (abs(pos.y) < 50.f) {
        scale = 0.5f;
    }

    if (abs(pos.y) < 10.f) {
        scale = 0.1f;
    }

    pos.y = 0;
    pos.x = glm::round(pos.x / (100.f * scale)) * (100.f * scale);
    pos.z = glm::round(pos.z / (100.f * scale)) * (100.f * scale);

    v1.position = pos;
    v1.position.z = -1000 * scale + pos.z;
    v2.position = pos;
    v2.position.z = 1000 * scale + pos.z;

    v1.color = glm::vec3{0.3f, 0.3f, 0.7f};
    v2.color = glm::vec3{0.3f, 0.3f, 0.7f};

    for (int i = -10; i <= 10; i++) {
        v1.position.x = 100.f * scale * i + pos.x;
        v2.position.x = 100.f * scale * i + pos.x;
        framePacket.sendLine(v1, v2);
    }

    v1.position = pos;
    v1.position.x = -1000 * scale + pos.x;
    v2.position = pos;
    v2.position.x = 1000 * scale + pos.x;

    v1.color = glm::vec3{0.7f, 0.3f, 0.3f};
    v2.color = glm::vec3{0.7f, 0.3f, 0.3f};

    for (int i = -10; i <= 10; i++) {
        v1.position.z = 100.f * scale * i + pos.z;
        v2.position.z = 100.f * scale * i + pos.z;
        framePacket.sendLine(v1, v2);
    }
}

void
jleSceneEditorWindow::EditTransform(float *cameraView,
                                    float *cameraProjection,
                                    float *matrix,
                                    bool editTransformDecomposition)
{
    if (editTransformDecomposition) {
        if (ImGui::IsWindowFocused()) {
            if (ImGui::IsKeyPressed(ImGuiKey_T) && !ImGuizmo::IsUsing())
                _currentGizmoOperation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_R) && !ImGuizmo::IsUsing())
                _currentGizmoOperation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed(ImGuiKey_Z) && !ImGuizmo::IsUsing())
                _currentGizmoOperation = ImGuizmo::SCALE;
            if (ImGui::IsKeyPressed(ImGuiKey_U) && !ImGuizmo::IsUsing())
                _currentGizmoOperation = ImGuizmo::UNIVERSAL;
        }
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
            if (ImGui::RadioButton("Local", _currentGizmoMode == ImGuizmo::LOCAL))
                _currentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", _currentGizmoMode == ImGuizmo::WORLD))
                _currentGizmoMode = ImGuizmo::WORLD;
        }

        ImGui::Checkbox("##UseSnap", &_useSnap);
        ImGui::SameLine();

        const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

        ImGui::PushItemWidth(150.f * globalImguiScale);
        switch (_currentGizmoOperation) {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &_snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &_snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &_snap[0]);
            break;
        }
        ImGui::Checkbox("Bound Sizing", &_boundSizing);
        if (_boundSizing) {
            ImGui::PushID(3);
            ImGui::Checkbox("##BoundSizing", &_boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", _boundsSnap);
            ImGui::PopID();
        }
        ImGui::PopItemWidth();
    }

    ImGuizmo::Manipulate(cameraView,
                         cameraProjection,
                         _currentGizmoOperation,
                         _currentGizmoMode,
                         matrix,
                         NULL,
                         _useSnap ? &_snap[0] : NULL,
                         _boundSizing ? _bounds : NULL,
                         _boundSizingSnap ? _boundsSnap : NULL);
}