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

#include "jleGameEngine.h"
#include "modules/jleEditorUpdateContext.h"

#include <string>
#include <vector>

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
struct jleWindowResizeEvent;
class jleSerializationContext;

class jleEditor : public jleGameEngine
{
public:
    jleEditor(EngineConstructConfig& config);

    ~jleEditor() override;

    void start(jleEngineUpdateContext &context) override;

    void render(jleCamera& camera, jleEngineUpdateContext &ctx, wi::jobsystem::context &jobsCtx) override;

    void update(jleEngineUpdateContext &ctx) override;

    jleEditorGizmos &gizmos();

    jleEditorSaveState &saveState();

    void updateEditorLoadedScenes(jleEngineUpdateContext &ctx);

    //std::vector<std::shared_ptr<jleScene>> &getEditorScenes();

    //jleEditorTextEdit &editorTextEdit();

    //jleEditorSceneObjectsWindow &getEditorSceneObjectsWindow();

    //jleResourceIndexer &resourceIndexer();

    bool checkSceneIsActiveEditor(const std::string &sceneName);

    //std::shared_ptr<jleScene> loadScene(const jlePath &scenePath,
    //                                    jleEngineUpdateContext &ctx,
    //                                    bool startObjects = true);

private:
    struct jleEditorInternal;
    std::unique_ptr<jleEditorInternal> _internal;

    void exiting() override;

    void renderGameView(const jleFramePacket& framePacketIn, jleFramebufferInterface& framebufferOut);

    void renderEditorSceneView(jleEditorUpdateContext &ctx);

    void renderEditorUI(jleEditorUpdateContext& ctx);

    void initImgui();

    void renderEditorGizmos(jleFramePacket &renderGraph, jleGameRuntime &gameRuntime);

    void renderEditorGizmosObject(jleObject *object, jleFramePacket &renderGraph);

    void mainEditorWindowResized(const jleWindowResizeEvent &resizeEvent);

    class jleEditorWindows;
    std::unique_ptr<jleEditorWindows> _editorWindows{};

    std::vector<std::shared_ptr<jleScene>> _editorScenes;

    std::shared_ptr<jleSceneEditorWindow> _sceneWindow;

    std::shared_ptr<jleEditorSceneObjectsWindow> _editorSceneObjects;

    std::unique_ptr<jleResourceIndexer> _resourceIndexer;

    std::shared_ptr<jleEditorTextEdit> _textEditWindow;

    std::unique_ptr<jleEditorGizmos> _gizmos;
};

#endif // JLE_BUILD_EDITOR
