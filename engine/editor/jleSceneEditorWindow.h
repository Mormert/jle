// Copyright (c) 2023. Johan Lind

#pragma once

#include "editor/jleEditorImGuiWindowInterface.h"
#include <ImGui/ImGuizmo.h>

#include "jleGameEngine.h"
#include "jleImage.h"

class jleSceneEditorWindow : public iEditorImGuiWindow {
public:
    jleSceneEditorWindow(const std::string &window_name, std::shared_ptr<jleFramebufferInterface> &framebuffer);

    void update(jleGameEngine &ge) override;

private:
    float _lastGameWindowWidth = 0.f, _lastGameWindowHeight = 0.f;
    std::pair<int32_t, int32_t> _lastCursorPos;
    bool _wasFocused = false;

    std::unique_ptr<jleFramebufferInterface> _pickingFramebuffer;

    std::shared_ptr<jleFramebufferInterface> _framebuffer;

    jleCameraSimpleFPVController _fpvCamController;

    ImGuizmo::OPERATION _currentGizmoOperation{ImGuizmo::TRANSLATE};
    void EditTransform(float *cameraView, float *cameraProjection, float *matrix, bool editTransformDecomposition);
};
