#include "jleGame.h"
#include <fstream>

namespace jle {
    void jleGame::UpdateActiveScenes(float dt) {
        for (int i = mActiveScenes.size() - 1; i >= 0; i--) {
            if (mActiveScenes[i]->bPendingSceneDestruction) {
                mActiveScenes.erase(mActiveScenes.begin() + i);
                continue;
            }

            mActiveScenes[i]->SceneUpdate();
            mActiveScenes[i]->ProcessNewSceneObjects();
            mActiveScenes[i]->UpdateSceneObjects(dt);
        }
    }

    std::vector<std::shared_ptr<jleScene>> &jleGame::GetActiveScenesRef() {
        return mActiveScenes;
    }

    bool jleGame::CheckSceneIsActive(const std::string &sceneName) {
        for (auto &&scene: mActiveScenes) {
            if (sceneName == scene->mSceneName) {
                return true;
            }
        }

        return false;
    }

    std::shared_ptr<jleScene> jleGame::LoadScene(const std::string &scenePath) {

        if (CheckSceneIsActive(scenePath)) {
            LOG_WARNING << "Loaded scene is already loaded";
            return nullptr;
        }

        std::ifstream i(scenePath);
        if (i.good()) {
            std::shared_ptr<jleScene> scene = CreateScene<jle::jleScene>();
            nlohmann::json j;
            i >> j;

            jle::from_json(j, *scene);
            return scene;
        } else {
            LOG_ERROR << "Could not load scene with path: " << scenePath;
            return nullptr;
        }
    }
}