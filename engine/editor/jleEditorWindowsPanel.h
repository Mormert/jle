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

    struct jleEditorWindowsPanelRenderContext
    {
        const jleFrameInfo& frameInfo;
        jleGameEngine& gameEngine;
        jleWindow& window;
    };

    explicit jleEditorWindowsPanel(const std::string &window_name, jleResources& resources, jleEngineSettings& settings);

    void renderUI(const jleEditorWindowsPanelRenderContext& context);

    void addWindow(std::shared_ptr<jleEditorWindowInterface> window);

    inline void menuButtonsupdate(const jleEditorWindowsPanelRenderContext& context);

    inline void dockspaceupdate(const jleEditorWindowsPanelRenderContext& context);

private:
    std::shared_ptr<jleTexture> _crossIcon;           // X
    std::shared_ptr<jleTexture> _maximizeIcon;        // [ ]
    std::shared_ptr<jleTexture> _minimizeIcon;        // -
    std::shared_ptr<jleTexture> _jleIcon;

    std::vector<std::shared_ptr<jleEditorWindowInterface>> windows;

    jleEditorGameControllerWidget _gameController;
};

#endif // JLE_BUILD_EDITOR
