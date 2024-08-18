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
#include "jleGame.h"
#include "jlePhysics.h"
#include "jleTexture.h"
#include "modules/graphics/core/jleFramebufferMultisample.h"
#include "modules/graphics/core/jleFramebufferPicking.h"
#include "modules/graphics/core/jleIncludeGL.h"
#include "modules/graphics/jle3DGraph.h"
#include "modules/graphics/jle3DRenderer.h"
#include "modules/graphics/jle3DSettings.h"
#include "modules/graphics/runtime/components/cMesh.h"
#include "modules/input/jleInput.h"
#include "modules/physics/components/cRigidbody.h"
#include "modules/windowing/jleWindow.h"
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

    auto pos = gEditor->camera().getPosition();
    fpvCamController.position = pos;
}

void
jleSceneEditorWindow::renderUI(jleGameEngine &ge)
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
    _lastCursorPos = gEngine->window().cursor();
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

    const auto mouseCoordinateX = getPixelatedMousePosX() + static_cast<int>(gEditor->camera().getPosition().x);
    const auto mouseCoordinateY = getPixelatedMousePosY() + static_cast<int>(gEditor->camera().getPosition().y);

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == _lastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == _lastGameWindowHeight)) {
        _lastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        _lastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

        const auto aspect = static_cast<float>(_lastGameWindowHeight) / static_cast<float>(_lastGameWindowWidth);
        auto dims = jleFramebufferInterface::fixedAxisDimensions(
            jleFramebufferInterface::FIXED_AXIS::width, aspect, static_cast<unsigned int>(ImGui::GetWindowHeight()));

        _framebuffer->resize(_lastGameWindowWidth, _lastGameWindowHeight);
    }

    const auto &selectedObject = gEditor->getEditorSceneObjectsWindow().GetSelectedObject();

    glBindTexture(GL_TEXTURE_2D, (unsigned int)_framebuffer->texture());

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

        gEngine->renderer().renderMeshesPicking(*_pickingFramebuffer, gEditor->camera(), gEngine->renderGraph());

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
                auto &game = ((jleGameEngine *)gEngine)->gameRef();
                scenes = game.activeScenesRef();
            }

            scenes.insert(scenes.end(), gEditor->getEditorScenes().begin(), gEditor->getEditorScenes().end());

            for (auto &scene : scenes) {
                for (auto &object : scene->sceneObjects()) {
                    std::shared_ptr<jleObject> o{};
                    object->tryFindChildWithInstanceId(pickedID, o);
                    if (o) {
                        gEditor->getEditorSceneObjectsWindow().SetSelectedObject(o);
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

    if (gEditor->perspectiveCamera) {
        if (ImGui::Button("Orthographic")) {
            gEditor->perspectiveCamera = false;
            ImGuizmo::SetOrthographic(false);
        }
    } else {
        if (ImGui::Button("Perspective")) {
            gEditor->perspectiveCamera = true;
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

    if (!gEngine->isGameKilled()) {
        if (auto &&scene = gEditor->getEditorSceneObjectsWindow().GetSelectedScene().lock()) {
            ImGui::Checkbox("Physics Debug", &scene->getPhysics().renderDebugEnabled);
        }
    }

    ImGui::SameLine();

    if (gEditor->camera().getProjectionType() == jleCameraProjection::Perspective) {
        ImGui::Text("[%d, %d] (%f)", _framebuffer->width(), _framebuffer->height(), cameraSpeed);
    } else {
        ImGui::Text("[%d, %d - Ortho Zoom: %f] (%f)",
                    _framebuffer->width(),
                    _framebuffer->height(),
                    orthoZoomValue,
                    cameraSpeed);
    }

    const float *viewMatrix = &gEditor->camera().getViewMatrix()[0][0];
    const float *projectionMatrix = &gEditor->camera().getProjectionMatrix()[0][0];
    static const auto identityMatrix = glm::mat4{1.f};
    const static float *identityMatrixPtr = &identityMatrix[0][0];
    // ImGuizmo::DrawGrid(viewMatrix, projectionMatrix, identityMatrixPtr, 25.f);

    // The following commented code is for a camera controller "cube" in the top left corner.
    // It contains a bug, however, and is not really usable at the moment.

    /*static bool isManipulating = false;
    static glm::mat4 manipulatingMatrix{1.f};
    static glm::mat4 lastFrameMatrix{1.f};

    if (!isManipulating) {
        manipulatingMatrix = gEditor->camera().getViewMatrix();
    }

    ImGuizmo::ViewManipulate(
        &manipulatingMatrix[0][0],
        250.f,
        ImVec2(ImGui::GetWindowPos().x + 20 * globalImguiScale, ImGui::GetWindowPos().y + 35 * globalImguiScale),
        ImVec2(128 * globalImguiScale, 128 * globalImguiScale),
        0x10101010);

    if (manipulatingMatrix != gEditor->camera().getViewMatrix()) {
        // The view manipulate function modified the view matrix
        isManipulating = true;
    }

    if (isManipulating) {
        if (lastFrameMatrix == manipulatingMatrix) {
            isManipulating = false;
        }
    }

    if (isManipulating) {
        auto pos = glm::vec3(manipulatingMatrix[3]);
        gEditor->camera().setViewMatrix(manipulatingMatrix, pos);
        lastFrameMatrix = manipulatingMatrix;
    }*/

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
                    rb->setWorldMatrixAndScaleRigidbody(worldMatrixBefore);
                } else {
                    obj->getTransform().setWorldMatrix(worldMatrixBefore);
                }
            } else {
                obj->getTransform().setWorldMatrix(worldMatrixBefore);
            }
        }

        if (auto meshComponent = obj->getComponent<cMesh>()) {
            if (auto mesh = meshComponent->getMesh()) {
                glm::mat4 modelMatrix = obj->getTransform().getWorldMatrix();
                glm::mat4 matrix1 = glm::scale(modelMatrix, glm::vec3{1.00514159265f});
                glm::mat4 matrix2 = glm::scale(modelMatrix, glm::vec3{0.99514159265f});
                auto material = gEditor->gizmos().selectedObjectMaterial();
                gEditor->renderGraph().sendMesh(mesh, material, matrix1, obj->instanceID(), false);
                gEditor->renderGraph().sendMesh(mesh, material, matrix2, obj->instanceID(), false);
            }
        }
    }

    ImGui::EndGroup();

    // If window is hovered and Gizmo is not being moved/used
    if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing()) {
        auto t = ge.deltaFrameTime();
        auto dragDelta = ImGui::GetMouseDragDelta(1);

        if (gEditor->camera().getProjectionType() == jleCameraProjection::Perspective ||
            ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
            if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                fpvCamController.applyPerspectiveMouseMovementDelta(glm::vec2{mouseDeltaX, mouseDeltaY}, 300.f);
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }
        } else {
            fpvCamController.move(glm::vec3{dragDelta.x, dragDelta.y, 0.f} * t * 5.f);
        }

        if (ImGui::IsKeyDown(ImGuiKey_W)) {
            if (gEditor->camera().getProjectionType() == jleCameraProjection::Orthographic) {
                fpvCamController.moveUp(cameraSpeed * t);
            } else {
                fpvCamController.moveForward(cameraSpeed * t);
            }
        }
        if (ImGui::IsKeyDown(ImGuiKey_S)) {
            if (gEditor->camera().getProjectionType() == jleCameraProjection::Orthographic) {
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
            gEditor->camera().setViewMatrix(fpvCamController.getLookAtViewMatrix(), fpvCamController.position);
        }

        auto currentScroll = gEngine->input().mouse->scrollY();
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && currentScroll != 0.f) {
            orthoZoomValue -= currentScroll * 1.f * ge.deltaFrameTime();
            orthoZoomValue = glm::clamp(orthoZoomValue, 0.01f, 2.f);
        } else if (currentScroll != 0.f) {
            cameraSpeed += currentScroll * 200.f * ge.deltaFrameTime();
            cameraSpeed = glm::clamp(cameraSpeed, 0.2f, 500.f);
        }
    }

    ImGui::End();
}

void
jleSceneEditorWindow::render(jle3DGraph &graph, const jleEditorModulesContext &context)
{
    if (context.editor.perspectiveCamera) {
        gEditor->camera().setPerspectiveProjection(45.f, _framebuffer->width(), _framebuffer->height(), 10000.f, 0.1f);
    } else {
        gEditor->camera().setOrthographicProjection(
            _framebuffer->width() * orthoZoomValue, _framebuffer->height() * orthoZoomValue, 10000.f, -10000.f);
    }

    if (_framebuffer->width() != _msaa->width() || _framebuffer->height() != _msaa->height()) {
        _msaa->resize(_framebuffer->width(), _framebuffer->height());
    }

    context.engineModulesContext.rendererModule.render(
        *_msaa, gEditor->camera(), graph, context.engine.renderSettings(), context.engineModulesContext.resourcesModule);

    _msaa->blitToOther(*_framebuffer);
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
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }

        ImGui::Checkbox("##UseSnap", &useSnap);
        ImGui::SameLine();

        const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

        ImGui::PushItemWidth(150.f * globalImguiScale);
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
        ImGui::PopItemWidth();
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