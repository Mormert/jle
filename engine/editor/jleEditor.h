// Copyright (c) 2023. Johan Lind

#pragma once

#include "jlePathDefines.h"

#include "editor/jleEditorImGuiWindowInterface.h"
#include "jleGameEngine.h"
#include "jleResourceRef.h"
#include "jleImage.h"
#include "jleMesh.h"
#include <memory>
#include <vector>

class jleFramebufferInterface;

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

    static inline jleCameraProjection projectionType;

    static inline jleCamera editorCamera{jleCameraProjection::Orthographic};

    void updateEditorLoadedScenes(float dt);

    std::vector<std::shared_ptr<jleScene>> &getEditorScenes();

    bool
    checkSceneIsActiveEditor(const std::string &sceneName)
    {
        for (auto &&scene : _editorScenes) {
            if (sceneName == scene->sceneName) {
                return true;
            }
        }

        return false;
    }

    std::shared_ptr<jleScene>
    loadScene(const jlePath &scenePath, bool startObjects = true)
    {
        auto scene = gCore->resources().loadResourceFromFile<jleScene>(scenePath, true);

        auto it = std::find(_editorScenes.begin(), _editorScenes.end(), scene);
        if (it == _editorScenes.end()) {
            _editorScenes.push_back(scene);
            scene->onSceneCreation();
            if(startObjects)
            {
                scene->startObjects();
            }
        } else {
            LOG_WARNING << "Loaded scene is already loaded";
        }

        return scene;
    }

    jleResourceRef<jleMesh> pointLightLampGizmoMesh;
    jleResourceRef<jleMesh> directionalLightLampGizmoMesh;

private:
    void renderGameView();

    void renderEditorSceneView();

    void renderEditorUI();

    void initImgui();

    void imguiTheme();

    void renderEditorGizmos();

    void renderEditorGridGizmo();

    void renderEditorGizmosObject(jleObject* object);

    void addImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window);

    void mainEditorWindowResized(int w, int h);

    std::vector<std::shared_ptr<iEditorImGuiWindow>> _imGuiWindows;

    std::vector<std::shared_ptr<jleScene>> _editorScenes;
};
