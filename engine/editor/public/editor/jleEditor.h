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

#include "jleGameEngine.h"
#include "modules/jleEditorUpdateContext.h"

#include <string>
#include <vector>

class jleEditorWindow;
class jleFileWatcher;
class jleEditorWindowInterface;
class jleFramebufferInterface;
class jleSceneEditorWindow;
class jleEditorTextEdit;
class jleEditorSceneObjectsWindow;
class jleCamera;
class jleScene;
class jleEditorGizmos;
class jleEditorSaveState;
class jlePath;
class jleObject;
class jleResourceIndexer;
class jleSerializationContext;
struct jleWindowDimensions;

namespace jlECS
{
class ECS;
namespace Debug
{
class ECS_Debug;
}
}

class jleEditor
{
public:
    struct EditorConstructConfig{
        std::function<void(jleGameModules&, jleEngineUpdateContext&, jlECS::ECS&)> modulesUpdateRenderablesOnly = {};
        std::function<void(jleEditorUpdateContext&)> updateEditorGameModules = {};
    };

    explicit jleEditor(const EditorConstructConfig &editorConfig, const jleGameEngine::EngineConstructConfig &engineConfig);

    ~jleEditor();

    void run();

    void render(jleCamera& camera, jleEngineUpdateContext &ctx, wi::jobsystem::context &jobsCtx);

    jleEditorGizmos &gizmos();

    jleEditorSaveState &saveState();
private:
    struct jleEditorInternal;
    std::unique_ptr<jleEditorInternal> _internal;

    jleGameModules* getCurrentGameModules();

    void init();

    void mainEditorLoop();

    void exiting();

    void renderEditorSceneView(jleEditorUpdateContext &ctx);

    void renderEditorUI(jleEditorUpdateContext& ctx);

    void initImgui();

    void mainEditorWindowResized(const jleWindowDimensions &windowDimensions);

    class jleEditorWindows;

    std::unique_ptr<jleEditorWindow> _mainEditorWindow;

    std::unique_ptr<jleEditorWindows> _editorWindows{};

    std::shared_ptr<jleSceneEditorWindow> _sceneWindow;

    std::shared_ptr<jleEditorSceneObjectsWindow> _editorSceneObjects;

    std::unique_ptr<jleResourceIndexer> _resourceIndexer;

    std::shared_ptr<jleEditorTextEdit> _textEditWindow;

    std::unique_ptr<jleEditorGizmos> _gizmos;

    std::unique_ptr<jlECS::Debug::ECS_Debug> _editorEcs;
    std::unique_ptr<jleGameModules> _editorModules;

    EditorConstructConfig _editorConstructConfig;

    bool _editorRunning = false;

    jleGameEngine _gameEngine;
};
