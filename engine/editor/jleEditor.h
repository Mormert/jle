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
#include "modules/jleEditorModulesContext.h"

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
    jleEditor();

    ~jleEditor() override;

    void start(jleEngineModulesContext &context) override;

    void render(jleEngineModulesContext &ctx, wi::jobsystem::context &jobsCtx) override;

    void update(jleEngineModulesContext &ctx) override;

    // std::shared_ptr<jleFramebufferInterface> editorScreenFramebuffer;

    jleEditorGizmos &gizmos();

    jleEditorSaveState &saveState();

    void updateEditorLoadedScenes(jleEngineModulesContext &ctx);

    std::vector<std::shared_ptr<jleScene>> &getEditorScenes();

    jleEditorTextEdit &editorTextEdit();

    jleEditorSceneObjectsWindow &getEditorSceneObjectsWindow();

    jleResourceIndexer &resourceIndexer();

    bool checkSceneIsActiveEditor(const std::string &sceneName);

    std::shared_ptr<jleScene> loadScene(const jlePath &scenePath,
                                        jleEngineModulesContext &ctx,
                                        bool startObjects = true);

private:
    struct jleEditorInternal;
    std::unique_ptr<jleEditorInternal> _internal;

    std::unique_ptr<jleEditorModulesContext> _editorContext;

    void exiting() override;

    void renderGameView(jleGameRuntime &runtime, jleSerializationContext &ctx);

    void renderEditorSceneView(jleEngineModulesContext &ctx);

    void renderEditorUI();

    void initImgui();

    void renderEditorGizmos(jleFramePacket &renderGraph, jleGameRuntime &gameRuntime);

    void renderEditorGizmosObject(jleObject *object, jleFramePacket &renderGraph);

    void addImGuiWindow(std::shared_ptr<jleEditorWindowInterface> window);

    void mainEditorWindowResized(const jleWindowResizeEvent &resizeEvent);

    class jleEditorWindows;
    std::unique_ptr<jleEditorWindows> _editorWindows{};

    std::vector<std::shared_ptr<jleEditorWindowInterface>> _imGuiWindows;

    std::vector<std::shared_ptr<jleScene>> _editorScenes;

    std::shared_ptr<jleSceneEditorWindow> _sceneWindow;

    std::shared_ptr<jleEditorSceneObjectsWindow> _editorSceneObjects;

    std::unique_ptr<jleResourceIndexer> _resourceIndexer;

    std::shared_ptr<jleEditorTextEdit> _textEditWindow;

    std::unique_ptr<jleEditorGizmos> _gizmos;
};

#endif // JLE_BUILD_EDITOR
