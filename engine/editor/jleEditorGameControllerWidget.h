// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleEditorImGuiWindowInterface.h"

class jleEditorGameControllerWidget : public iEditorImGuiWindow {
public:
    explicit jleEditorGameControllerWidget(const std::string& window_name);

    void Update(jleGameEngine& ge) override;

    std::shared_ptr<jleTextureInterface> _playGameIcon;
    std::shared_ptr<jleTextureInterface> _restartGameIcon;
    std::shared_ptr<jleTextureInterface> _pauseGameIcon;
    std::shared_ptr<jleTextureInterface> _nextFrameIcon;
};
