// Copyright (c) 2022. Johan Lind

#pragma once

#include "jlePathDefines.h"

#include "editor/jleEditorImGuiWindowInterface.h"
#include "editor/jleEditorSettings.h"
#include "jleGameEngine.h"
#include "jleImage.h"
#include <memory>
#include <vector>

class jleFramebuffer;

class jleEditor : public jleGameEngine {
public:
    jleEditor(std::shared_ptr<jleGameSettings>,
              std::shared_ptr<jleEditorSettings>);

    void StartEditor();

    void Start() override;

    void Render() override;

    std::shared_ptr<jleFramebuffer> _editorFramebuffer;

    static inline jleCamera _editorCamera{jleCameraProjection::Orthographic};

private:
    void InitImgui();

    void SetImguiTheme();

    void AddImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window);

    void MainEditorWindowResized(int w, int h);

    std::vector<std::shared_ptr<iEditorImGuiWindow>> ImGuiWindows;

    std::shared_ptr<jleEditorSettings> editor_settings;
};
