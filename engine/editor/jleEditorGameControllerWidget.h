// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleEditorImGuiWindowInterface.h"
#include "jleTexture.h"

class jleEditorGameControllerWidget : public iEditorImGuiWindow {
public:
    explicit jleEditorGameControllerWidget(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    std::shared_ptr<jleTexture> _playGameIcon;
    std::shared_ptr<jleTexture> _restartGameIcon;
    std::shared_ptr<jleTexture> _pauseGameIcon;
    std::shared_ptr<jleTexture> _nextFrameIcon;
};
