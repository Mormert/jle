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

class jleEditorWindowsPanel : public jleEditorWindowInterface
{
public:
    explicit jleEditorWindowsPanel(const std::string &window_name,
                                   jleResources &resources,
                                   jleEngineSettings &settings);

    void renderUI(jleEngineModulesContext &ctx);

    void addWindow(std::shared_ptr<jleEditorWindowInterface> window);

    inline void menuButtonsupdate(jleEngineModulesContext &ctx);

    inline void dockspaceupdate(jleEngineModulesContext &ctx);

private:
    std::shared_ptr<jleTexture> _crossIcon;    // X
    std::shared_ptr<jleTexture> _maximizeIcon; // [ ]
    std::shared_ptr<jleTexture> _minimizeIcon; // -
    std::shared_ptr<jleTexture> _jleIcon;

    std::vector<std::shared_ptr<jleEditorWindowInterface>> windows;

    jleEditorGameControllerWidget _gameController;
};

#endif // JLE_BUILD_EDITOR
