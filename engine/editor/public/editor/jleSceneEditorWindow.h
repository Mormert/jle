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

#pragma once

#include <core/jleCommon.h>
#include <core/jleCamera.h>
#include <jlECS/jlECS.h>
#include "editor/jleEditorImGuiWindowInterface.h"


#include <ImGui/ImGuizmo.h>

class jleEditorUpdateContext;
class jleFramebufferMultisample;
class jlePhysics;
class jleUndoRedoManager;

class jleSceneEditorWindow : public jleEditorWindowInterface
{
public:
    explicit jleSceneEditorWindow(const std::string &window_name);

    struct RenderUIInput{
        jleEditorUpdateContext &editorUpdate;
        jlECS::ECS& ecs;
        std::shared_ptr<std::vector<jlECS::ObjectRef>> selectedObjects;
        jlePhysics& physics;
        jleUndoRedoManager& undoRedo;
    };

    void renderUI(const RenderUIInput& input);

    void renderEditorGrid(jleFramePacket &graph);

    void render(jleFramePacket &framePacket, const jleEditorUpdateContext &context);

    jleCameraSimpleFPVController fpvCamController{};
    float cameraSpeed = 100.f;
    float orthoZoomValue = 10.f;

    [[nodiscard]] glm::vec3 getCameraPosition() { return _renderCamera.getPosition(); }
private:
    float _lastGameWindowWidth = 0.f, _lastGameWindowHeight = 0.f;
    std::pair<int32_t, int32_t> _lastCursorPos;

    jleCamera _renderCamera{};
    bool _perspectiveCamera = true;

    std::unique_ptr<jleFramebufferInterface> _pickingFramebuffer;
    std::unique_ptr<jleFramebufferMultisample> _msaa;

    std::shared_ptr<jleFramebufferInterface> _framebuffer;

    bool _multiGizmoIsActive = false;
    glm::mat4 _multiGizmoInitialMatrix{1.f};
    glm::mat4 _multiGizmoCurrentMatrix{1.f};

    ImGuizmo::OPERATION _currentGizmoOperation{ImGuizmo::TRANSLATE};
    ImGuizmo::MODE _currentGizmoMode{ImGuizmo::LOCAL};
    bool _useSnap = false;
    float _snap[3] = {1.f, 1.f, 1.f};
    float _bounds[6] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
    float _boundsSnap[3] = {0.1f, 0.1f, 0.1f};
    bool _boundSizing = false;
    bool _boundSizingSnap = false;

    void EditTransform(float *cameraView, float *cameraProjection, float *matrix, bool editTransformDecomposition);
};
