// Copyright (c) 2023. Johan Lind

#pragma once

#include "jlePathDefines.h"

#include "editor/jleEditorImGuiWindowInterface.h"
#include "jleGameEngine.h"
#include "jleImage.h"
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
    checkSceneIsActiveEditor(const std::string &sceneName) {
        for (auto &&scene : _editorScenes) {
            if (sceneName == scene->sceneName) {
                return true;
            }
        }

        return false;
    }

    std::shared_ptr<jleScene>
    loadScene(const std::string &scenePath)
    {
        if (checkSceneIsActiveEditor(scenePath)) {
            LOG_WARNING << "Loaded scene is already loaded";
            return nullptr;
        }

        auto scene = jleScene::loadScene(scenePath);

        if(scene)
        {
            _editorScenes.push_back(scene);
        }

        return scene;
    }

    jleResourceRef<jleEngineSettings> &getEngineSettingsResourceRef();

private:
    void initImgui();

    void imguiTheme();

    void addImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window);

    void mainEditorWindowResized(int w, int h);

    void checkGlErrors();

    std::vector<std::shared_ptr<iEditorImGuiWindow>> _imGuiWindows;

    std::vector<std::shared_ptr<jleScene>> _editorScenes;
};
