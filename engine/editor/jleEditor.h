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

#include <vector>
#include <string>

class jleFileIndexer;
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
struct jleWindowResizeEvent;

class jleEditor;
inline jleEditor *gEditor;

class jleEditor : public jleGameEngine
{
public:
    jleEditor();

    ~jleEditor() override;

    void start() override;

    void render(wi::jobsystem::context& ctx) override;

    void update(float dt) override;

    std::shared_ptr<jleFramebufferInterface> editorScreenFramebuffer;

    jleCamera& camera();
    bool perspectiveCamera = true;

    jleEditorGizmos& gizmos();

    jleEditorSaveState& saveState();

    void updateEditorLoadedScenes(float dt);

    std::vector<std::shared_ptr<jleScene>> &getEditorScenes();

    jleEditorTextEdit &editorTextEdit();

    jleEditorSceneObjectsWindow &editorSceneObjects();

    jleFileIndexer& fileIndexer();

    bool
    checkSceneIsActiveEditor(const std::string &sceneName);

    std::shared_ptr<jleScene>
    loadScene(const jlePath &scenePath, bool startObjects = true);

private:
    struct jleEditorInternal;
    std::unique_ptr<jleEditorInternal> _internal;

    void exiting() override;

    void renderGameView();

    void renderEditorSceneView();

    void renderEditorUI();

    void initImgui();

    void renderEditorGizmos();

    void renderEditorGridGizmo();

    void renderEditorGizmosObject(jleObject *object);

    void addImGuiWindow(std::shared_ptr<jleEditorWindowInterface> window);

    void mainEditorWindowResized(const jleWindowResizeEvent& resizeEvent);

    std::vector<std::shared_ptr<jleEditorWindowInterface>> _imGuiWindows;

    std::vector<std::shared_ptr<jleScene>> _editorScenes;

    std::shared_ptr<jleSceneEditorWindow> _sceneWindow;

    std::shared_ptr<jleEditorSceneObjectsWindow> _editorSceneObjects;

    std::unique_ptr<jleFileIndexer> _fileIndexer;

    std::shared_ptr<jleEditorTextEdit> _textEditWindow;

    std::unique_ptr<jleCamera> _camera;

    std::unique_ptr<jleEditorGizmos> _gizmos;
};

#endif // JLE_BUILD_EDITOR
