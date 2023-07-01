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
class jleSceneEditorWindow;

class jleEditor;
inline jleEditor *gEditor;

struct jleEditorSaveState : public jleSerializedResource, public std::enable_shared_from_this<jleEditorSaveState> {
    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(loadedScenePaths),
                CEREAL_NVP(cameraPosition),
                CEREAL_NVP(gameRunning),
                CEREAL_NVP(orthographicCamera),
                CEREAL_NVP(cameraYaw),
                CEREAL_NVP(cameraPitch));
    }

    JLE_REGISTER_RESOURCE_TYPE(jleEditorSaveState, edsave);
    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedResource)

    std::vector<jlePath> loadedScenePaths{};
    glm::vec3 cameraPosition{};
    float cameraYaw{};
    float cameraPitch{};
    bool orthographicCamera{};
    bool gameRunning{};
};

CEREAL_REGISTER_TYPE(jleEditorSaveState)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleEditorSaveState)

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

    jleResourceRef<jleEditorSaveState> _editorSaveState;

    void exiting() override;

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

    std::shared_ptr<jleSceneEditorWindow> _sceneWindow;
};
