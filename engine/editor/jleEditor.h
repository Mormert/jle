// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "jleGameEngine.h"

#include <vector>

class jleFileChangeNotifier;
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

class jleEditor;
inline jleEditor *gEditor;

class jleEditor : public jleGameEngine
{
public:
    jleEditor();

    void start() override;

    void render() override;

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

    void mainEditorWindowResized(int w, int h);

    std::vector<std::shared_ptr<jleEditorWindowInterface>> _imGuiWindows;

    std::vector<std::shared_ptr<jleScene>> _editorScenes;

    std::shared_ptr<jleSceneEditorWindow> _sceneWindow;

    std::shared_ptr<jleEditorSceneObjectsWindow> _editorSceneObjects;

    std::unique_ptr<jleFileChangeNotifier> _fileChangeNotifier;

    std::shared_ptr<jleEditorTextEdit> _textEditWindow;

    std::unique_ptr<jleCamera> _camera;

    std::unique_ptr<jleEditorGizmos> _gizmos;
};

#endif // BUILD_EDITOR
