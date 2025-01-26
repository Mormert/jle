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
#include "game/jleGame.h"
#include "jleEditor.h"
#include "jleEditorGizmos.h"
#include "jleUndoRedo.h"
#include "modules/graphics/core/jleFramebufferMultisample.h"
#include "modules/graphics/core/jleFramebufferPicking.h"
#include "modules/graphics/core/jleIncludeGL.h"
#include "modules/graphics/jleFramePacket.h"
#include "modules/graphics/jleGraphics.h"
#include "modules/graphics/jleGraphicsModule.h"
#include "modules/mesh/components/cMesh.h"
#include "modules/hierarchy/components/cTransform.h"
#include "modules/input/jleInputModule.h"
#include "modules/physics/components/cRigidbody.h"
#include "modules/physics/jlePhysics.h"
#include "modules/windowing/jleWindowModule.h"
#include <modules/graphics/core/jleFramebufferScreen.h>

#include <ImGui/imgui.h>
#include <btBulletDynamicsCommon.h>
#include <glm/common.hpp>
#include <utility>
#include <modules/hierarchy/jleHierarchyFuncs.h>
#include <modules/hierarchy/components/cParent.h>
#include <modules/physics/editor/jlePhysicsModuleEditor.h>

namespace{

class MoveTransformsCommand : public jleUndoRedoCommandBase{
public:
    MoveTransformsCommand(std::vector<jlECS::ObjectRef> objects, std::vector<glm::mat4> initialTransforms, const glm::mat4& delta)
        :_objects(std::move(objects)), _initialTransforms(std::move(initialTransforms)), _delta(delta) {}

    void execute(const CommandContext& ctx) override{
        for(auto& object : _objects){
            assert(object.isValid());
            glm::mat4 oldWorld = jleHierarchyFuncs::getWorldMatrix(object);
            glm::mat4 newWorld = _delta * oldWorld;

            jleHierarchyFuncs::setLocalMatrixFromWorld(object, newWorld);

            auto* transform = object.getComponentPtr<cTransform>();
            assert(transform);
            if (auto* rb = object.getComponentPtr<cRigidbody>())
            {
                if (auto* meshComp = object.getComponentPtr<cMesh>()) {
                    rb->updateRigidbodyScaling();
                }
            }
        }
    }

    void undo(const CommandContext& ctx) override{
        assert(_objects.size() == _initialTransforms.size());

        int i = 0;
        for(auto& object : _objects){
            auto transform = object.getComponentPtr<cTransform>();
            transform->setLocalMatrix(_initialTransforms[i]);
            i++;
        }
    }

private:
    std::vector<jlECS::ObjectRef> _objects;
    std::vector<glm::mat4> _initialTransforms;
    glm::mat4 _delta;
};

}

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
    ZoneScoped;

    if (!isOpened) {
        return;
    }

    const std::shared_ptr<std::vector<jlECS::ObjectRef>>& selectedObjects = input.selectedObjects;
    jlECS::ECS& ecs = input.ecs;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x,
                      ImGui::GetWindowPos().y,
                      (float)ImGui::GetWindowWidth(),
                      (float)ImGui::GetWindowHeight());

    constexpr float negYOffset = 8.0f;
    constexpr float negXOffset = 6.0f;

    const auto &cursorScreenPos = ImGui::GetCursorScreenPos();
    const auto viewport = ImGui::GetMainViewport();
    const int32_t windowPositionX = int32_t(cursorScreenPos.x) - (int32_t)viewport->Pos.x;
    const int32_t windowPositionY = int32_t(cursorScreenPos.y) - (int32_t)viewport->Pos.y;

    const auto previousFrameCursorPos = _lastCursorPos;
    _lastCursorPos = glm::ivec2{ImGui::GetMousePos().x, ImGui::GetMousePos().y};
    const glm::ivec2 mouseDelta = _lastCursorPos - previousFrameCursorPos;

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

    // If we're over a gizmo or an ImGui item, disallow picking.
    if ((ImGuizmo::IsOver() && !selectedObjects->empty()) ||
        (ImGui::IsAnyItemHovered() && ImGui::IsItemHovered())) {
        canSelectObject = false;
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && canSelectObject)
    {
        _isSelecting    = true;
        _selectStart = _lastCursorPos;
        _selectCurrent = _lastCursorPos;
    }

    if (_isSelecting && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        _selectCurrent = _lastCursorPos;
    }

    if (_isSelecting && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        _isSelecting = false;

        const int dragThreshold = 3; // minimal move to consider it a "box selection"
        int dragWidth  = std::abs(_selectCurrent.x - _selectStart.x);
        int dragHeight = std::abs(_selectCurrent.y - _selectStart.y);

        input.editorUpdate.getCurrentModules().getModule<jleGraphicsModule>()->getGraphics().renderMeshesPicking(*_pickingFramebuffer, _renderCamera, input.editorUpdate.editorFramePacket);
        _pickingFramebuffer->bind();

        GLint previousPackAlignment;
        glGetIntegerv(GL_PACK_ALIGNMENT, &previousPackAlignment);

        glFlush();
        glFinish();

        ImGuiIO& io = ImGui::GetIO();
        if (!io.KeyCtrl) {
            selectedObjects->clear();
        }

        if (dragWidth < dragThreshold && dragHeight < dragThreshold)
        {
            unsigned char data[3];
            int mouseY_flipped = (int)_lastGameWindowHeight - (_lastCursorPos.y - windowPositionY);

            int pixelReadX = (_lastCursorPos.x - windowPositionX) * (_pickingFramebuffer->width()  / _lastGameWindowWidth);
            int pixelReadY = mouseY_flipped            * (_pickingFramebuffer->height() / _lastGameWindowHeight);


            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glReadPixels(pixelReadX, pixelReadY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
            glPixelStorei(GL_PACK_ALIGNMENT, previousPackAlignment);

            int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;
            if (pickedID != 0x00ffffff) {
                if (ecs.isObjectAlive(pickedID)) {
                    auto obj = ecs.getObject(pickedID);
                    if (std::find(selectedObjects->begin(), selectedObjects->end(), obj) == selectedObjects->end()) {
                        selectedObjects->push_back(obj);
                    }
                }
            } else {
                if (!io.KeyCtrl) {
                    selectedObjects->clear();
                }
            }
        }
        else {
            // Box picking

            int boxMinX = std::min(_selectStart.x, _selectCurrent.x);
            int boxMinY = std::min(_selectStart.y, _selectCurrent.y);
            int boxMaxX = std::max(_selectStart.x, _selectCurrent.x);
            int boxMaxY = std::max(_selectStart.y, _selectCurrent.y);

            // Clip to the game window, if needed
            boxMinX = std::max(boxMinX, windowPositionX);
            boxMinY = std::max(boxMinY, windowPositionY);
            boxMaxX = std::min(boxMaxX, windowPositionX + (int)_lastGameWindowWidth);
            boxMaxY = std::min(boxMaxY, windowPositionY + (int)_lastGameWindowHeight);

            int readWidth  = boxMaxX - boxMinX;
            int readHeight = boxMaxY - boxMinY;
            if (readWidth <= 0 || readHeight <= 0) {
                _pickingFramebuffer->bindDefault();
                ImGui::End();
                return;
            }

            int flippedY = (int)_lastGameWindowHeight - (boxMaxY - windowPositionY);
            float scaleX = float(_pickingFramebuffer->width())  / float(_lastGameWindowWidth);
            float scaleY = float(_pickingFramebuffer->height()) / float(_lastGameWindowHeight);

            int pixelReadX = int((boxMinX - windowPositionX) * scaleX);
            int pixelReadY = int(flippedY * scaleY);
            int pixelReadWidth  = int(readWidth  * scaleX);
            int pixelReadHeight = int(readHeight * scaleY);

            if (pixelReadWidth <= 0 || pixelReadHeight <= 0) {
                LOGE << "Picking invalid read region. Skipping read.";
            } else if (pixelReadX < 0 || pixelReadY < 0 ||
                       (pixelReadX + pixelReadWidth > _pickingFramebuffer->width()) ||
                       (pixelReadY + pixelReadHeight > _pickingFramebuffer->height())) {
                LOGE << "Picking region out of FBO bounds. Skipping read.";
                       } else {
                           std::vector<unsigned char> data(pixelReadWidth * pixelReadHeight * 3);

                           glPixelStorei(GL_PACK_ALIGNMENT, 1);
                           glReadPixels(pixelReadX,
                                         pixelReadY,
                                         pixelReadWidth,
                                         pixelReadHeight,
                                        GL_RGB,
                                        GL_UNSIGNED_BYTE,
                                        data.data());
                           glPixelStorei(GL_PACK_ALIGNMENT, previousPackAlignment);

                           std::unordered_set<int> pickedIDs;
                           for (int i = 0; i < pixelReadWidth * pixelReadHeight; i++)
                           {
                               int r = data[i*3 + 0];
                               int g = data[i*3 + 1];
                               int b = data[i*3 + 2];
                               int id = (r) + (g << 8) + (b << 16);
                               if (id != 0x00ffffff) {
                                   pickedIDs.insert(id);
                               }
                           }

                           // Add them all to selected objects (unless already present)
                           for (auto id : pickedIDs) {
                               if (!ecs.isObjectAlive(id)) {
                                   continue;
                               }
                               auto obj = ecs.getObject(id);
                               if (std::find(selectedObjects->begin(), selectedObjects->end(), obj) == selectedObjects->end()) {
                                   selectedObjects->push_back(obj);
                               }
                           }
                       }

            // Return to default framebuffer
            _pickingFramebuffer->bindDefault();
        }
    }

    if (_isSelecting) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 vpPos = viewport->Pos; // absolute screen coords of the main viewport
        ImVec2 start(vpPos.x + _selectStart.x,    vpPos.y + _selectStart.y);
        ImVec2 end  (vpPos.x + _selectCurrent.x,  vpPos.y + _selectCurrent.y);

        drawList->AddRectFilled(start, end, IM_COL32(0, 0, 255, 50));   // Translucent fill
        drawList->AddRect(start, end,IM_COL32(0, 0, 255, 255));         // Solid outline
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
            _renderCamera.setViewMatrix(fpvCamController.getLookAtViewMatrix());
        }

        ImGui::SameLine();
        if (!input.editorUpdate.engineUpdateContext.gameRuntime.isGameKilled()) {

            auto& modules = input.editorUpdate.engineUpdateContext.gameRuntime.getGame().getModules();
            if (auto* physicsEditorModule = modules.getModule<jlePhysicsModuleEditor>()) {
                bool& debugRenderPhysics = physicsEditorModule->getDebugRenderingEnabledRef();
                ImGui::Checkbox("Physics Debug", &debugRenderPhysics);
            }
        }

        ImGui::SameLine();
        if (_renderCamera.getProjectionType() == jleCameraProjection::Perspective) {
            ImGui::Text("[%d, %d] (%f)", _framebuffer->width(), _framebuffer->height(), cameraSpeed);
        } else {
            ImGui::Text("[%d, %d - Ortho Zoom: %f] (%f)", _framebuffer->width(), _framebuffer->height(), orthoZoomValue, cameraSpeed);
        }
    }


    const float *viewMatrix = &_renderCamera.getViewMatrix()[0][0];
    const float *projectionMatrix = &_renderCamera.getProjectionMatrix()[0][0];

    if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        auto t = input.editorUpdate.engineUpdateContext.frameInfo.getDeltaTime();
        auto dragDelta = ImGui::GetMouseDragDelta(1);

        if (_renderCamera.getProjectionType() == jleCameraProjection::Perspective ||
            ImGui::IsKeyDown(ImGuiKey_LeftShift))
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                fpvCamController.applyPerspectiveMouseMovementDelta(glm::vec2{mouseDelta}, 300.f);
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

        _renderCamera.setViewMatrix(fpvCamController.getLookAtViewMatrix());

        float currentScroll = ImGui::GetScrollY();
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
        std::vector<std::pair<cTransform*, int>> transforms;
        transforms.reserve(selectedObjects->size());
        for (auto& objRef : *selectedObjects)
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
                avgPos += jleHierarchyFuncs::getWorldPosition(ecs.getObject(objectIndex));
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

                std::vector<glm::mat4> initialTransforms;
                std::vector<jlECS::ObjectRef> selectedObjectsWithTransforms;
                for (auto [transform, objectIndex] : transforms){
                    initialTransforms.push_back(transform->getLocalMatrix());
                    selectedObjectsWithTransforms.push_back(ecs.getObject(objectIndex));
                }

                jleUndoRedoCommandBase::CommandContext undoRedoCommandCtx = {input.editorUpdate.engineUpdateContext.serializationContext};

                auto command = std::make_unique<MoveTransformsCommand>(selectedObjectsWithTransforms, initialTransforms, delta);
                input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
            }

            //// Draw highlight
            //if (meshComponent) {
            //    if (auto mesh = meshComponent->getMesh()) {
            //        glm::mat4 modelMatrix = transform->getWorldMatrix();
            //        glm::mat4 matrix1 = glm::scale(modelMatrix, glm::vec3{1.00514159265f});
            //        glm::mat4 matrix2 = glm::scale(modelMatrix, glm::vec3{0.99514159265f});
            //        auto material = editorUpdate.gizmos.selectedObjectMaterial();
            //
            //        editorUpdate.engineUpdateContext.currentFramePacket.sendMesh(
            //            mesh, material, matrix1, selObj.objectIndex(), false);
            //        editorUpdate.engineUpdateContext.currentFramePacket.sendMesh(
            //            mesh, material, matrix2, selObj.objectIndex(), false);
            //    }
            //}

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
        _renderCamera.setOrthographicProjection(_framebuffer->width() * orthoZoomValue, _framebuffer->height() * orthoZoomValue, 10000.f, -10000.f);
    }

    if (_framebuffer->width() != _msaa->width() || _framebuffer->height() != _msaa->height()) {
        _msaa->resize(_framebuffer->width(), _framebuffer->height());
    }

    framePacket.camera = _renderCamera;
    ctx.getCurrentModules().getModule<jleGraphicsModule>()->getGraphics().render(*_msaa, framePacket);

    _msaa->blitToOther(*_framebuffer);
}

void
jleSceneEditorWindow::renderEditorGrid(jleFramePacket &framePacket)
{
    ZoneScoped;

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
            if (ImGui::IsKeyPressed(ImGuiKey_K) && !ImGuizmo::IsUsing())
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