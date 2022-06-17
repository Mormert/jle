// Copyright (c) 2022. Johan Lind

#pragma once

#include "iEditorImGuiWindow.h"

#include "jleGameEngine.h"
#include "iFramebuffer.h"

namespace jle {
    class SceneEditorWindow : public iEditorImGuiWindow {
    public:
        SceneEditorWindow(const std::string &window_name, std::shared_ptr<iFramebuffer> &framebuffer);

        void Update(jleGameEngine &ge) override;

    private:
        float mLastGameWindowWidth = 0.f, mLastGameWindowHeight = 0.f;
        std::pair<int32_t, int32_t> mLastCursorPos;
        bool mWasFocused = false;

        std::shared_ptr<iFramebuffer> mFramebuffer;
        Image mTransformMarkerImage;
        std::shared_ptr<iTexture> mTransformMarkerTexture;
        TexturedQuad mTexturedQuad;
    };
}