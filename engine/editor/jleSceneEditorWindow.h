// Copyright (c) 2022. Johan Lind

#pragma once

#include "editor/jleEditorImGuiWindowInterface.h"

#include "jleFramebufferInterface.h"
#include "jleGameEngine.h"

class jleSceneEditorWindow : public iEditorImGuiWindow {
public:
    jleSceneEditorWindow(const std::string& window_name,
                      std::shared_ptr<jleFramebufferInterface>& framebuffer);

    void Update(jleGameEngine& ge) override;

private:
    float mLastGameWindowWidth = 0.f, mLastGameWindowHeight = 0.f;
    std::pair<int32_t, int32_t> mLastCursorPos;
    bool mWasFocused = false;

    std::shared_ptr<jleFramebufferInterface> mFramebuffer;
    jleImage mTransformMarkerImage;
    std::shared_ptr<jleTextureInterface> mTransformMarkerTexture;
    TexturedQuad mTexturedQuad;
};
