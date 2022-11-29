// Copyright (c) 2022. Johan Lind

#pragma once

#include "jlePathDefines.h"

#include "editor/jleEditorImGuiWindowInterface.h"
#include "editor/jleEditorSettings.h"
#include "jleGameEngine.h"
#include "jleImage.h"
#include <memory>
#include <vector>

class jleFramebufferInterface;

class jleEditor : public jleGameEngine {
public:
    jleEditor(std::shared_ptr<jleGameSettings>, std::shared_ptr<jleEditorSettings>);

    void start() override;

    void render() override;

    std::shared_ptr<jleFramebufferInterface> editorScreenFramebuffer;

    static inline jleCameraProjection projectionType;

    static inline jleCamera editorCamera{jleCameraProjection::Orthographic};

private:
    void initImgui();

    void imguiTheme();

    void addImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window);

    void mainEditorWindowResized(int w, int h);

    std::vector<std::shared_ptr<iEditorImGuiWindow>> _imGuiWindows;

    std::shared_ptr<jleEditorSettings> _editorSettings;
};
