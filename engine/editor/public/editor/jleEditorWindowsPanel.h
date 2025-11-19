/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include "jleBuildConfig.h"

#if JLE_BUILD_EDITOR

#include "jleEditorGameControllerWidget.h"
#include "jleEditorImGuiWindowInterface.h"

class jleEditorUpdateContext;

class jleEditorWindowsPanel : public jleEditorWindowInterface
{
public:
    explicit jleEditorWindowsPanel(const std::string &window_name, jleSerializationContext& serializationContext, jleEngineSettings &settings);

    void renderUI(jleEditorUpdateContext &ctx);

    void addWindow(std::shared_ptr<jleEditorWindowInterface> window);

private:
    void menuButtonsUpdate(jleEditorUpdateContext &ctx);

    std::shared_ptr<jleTexture> _crossIcon;    // X
    std::shared_ptr<jleTexture> _maximizeIcon; // [ ]
    std::shared_ptr<jleTexture> _minimizeIcon; // -
    std::shared_ptr<jleTexture> _jleIcon;

    std::vector<std::shared_ptr<jleEditorWindowInterface>> windows;

    jleEditorGameControllerWidget _gameController;

    std::future<void> _tracyBuildFuture{};
    bool _tracyBuildDone{};
};

#endif // JLE_BUILD_EDITOR
