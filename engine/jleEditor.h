// Copyright (c) 2022. Johan Lind

#pragma once

#include "jlePathDefines.h"

#include "jleGameEngine.h"
#include "jleEditorSettings.h"
#include "iEditorImGuiWindow.h"

#include "Image.h"

#include <vector>
#include <memory>

namespace jle {
    class jleEditor : public jleGameEngine {
    public:

        jleEditor(std::shared_ptr<jleGameSettings>, std::shared_ptr<jleEditorSettings>);

        void StartEditor();

        void Start() override;

        void Render() override;

        std::shared_ptr<iFramebuffer> mEditorFramebuffer;

        static inline jleCamera mEditorCamera{jleCameraProjection::Orthographic};

    private:

        void InitImgui();

        void SetImguiTheme();

        void AddImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window);

        void MainEditorWindowResized(int w, int h);

        std::vector<std::shared_ptr<iEditorImGuiWindow>> ImGuiWindows;

        std::shared_ptr<jleEditorSettings> editor_settings;
    };

}


