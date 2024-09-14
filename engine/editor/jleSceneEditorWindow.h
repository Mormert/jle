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

#include "jleBuildConfig.h"

#if JLE_BUILD_EDITOR

#include "editor/jleEditorImGuiWindowInterface.h"

#include "jleCamera.h"
#include "jleGameEngine.h"

#include <ImGui/ImGuizmo.h>

class jleEditorModulesContext;
class jleFramebufferMultisample;

class jleSceneEditorWindow : public jleEditorWindowInterface
{
public:
    explicit jleSceneEditorWindow(const std::string &window_name);

    void renderUI(jleEditorModulesContext &ctx);

    void renderEditorGrid(jleFramePacket & graph);

    void render(jleFramePacket &graph, const jleEditorModulesContext &context);

    jleCameraSimpleFPVController fpvCamController{};
    float cameraSpeed = 100.f;
    float orthoZoomValue = 10.f;

    [[nodiscard]] glm::vec3 const
    getCameraPosition()
    {
        return _renderCamera.getPosition();
    }

private:

    float _lastGameWindowWidth = 0.f, _lastGameWindowHeight = 0.f;
    std::pair<int32_t, int32_t> _lastCursorPos;
    bool _wasFocused = false;

    jleCamera _renderCamera{};
    bool _perspectiveCamera = true;

    std::unique_ptr<jleFramebufferInterface> _pickingFramebuffer;
    std::unique_ptr<jleFramebufferMultisample> _msaa;

    std::shared_ptr<jleFramebufferInterface> _framebuffer;

    ImGuizmo::OPERATION _currentGizmoOperation{ImGuizmo::TRANSLATE};
    void EditTransform(float *cameraView, float *cameraProjection, float *matrix, bool editTransformDecomposition);
};

#endif // JLE_BUILD_EDITOR