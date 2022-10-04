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

    void startEditor();

    void start() override;

    void render() override;

    std::shared_ptr<jleFramebuffer> _editorFramebuffer;

    static inline jleCamera _editorCamera{jleCameraProjection::Orthographic};

private:
    void initImgui();

    void imguiTheme();

    void AddImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window);

    void mainEditorWindowResized(int w, int h);

    std::vector<std::shared_ptr<iEditorImGuiWindow>> ImGuiWindows;

    std::shared_ptr<jleEditorSettings> editor_settings;
};
