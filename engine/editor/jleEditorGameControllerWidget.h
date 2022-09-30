// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleEditorImGuiWindowInterface.h"

class jleEditorGameControllerWidget : public iEditorImGuiWindow {
public:
    explicit jleEditorGameControllerWidget(const std::string &window_name);

    void Update(jleGameEngine &ge) override;

    std::shared_ptr<jleTexture> mPlayGameIcon;
    std::shared_ptr<jleTexture> mRestartGameIcon;
    std::shared_ptr<jleTexture> mPauseGameIcon;
    std::shared_ptr<jleTexture> mNextFrameIcon;
};
