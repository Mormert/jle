// Copyright (c) 2022. Johan Lind

#include "jleGame.h"

void jleGame::UpdateActiveScenes(float dt) {
    JLE_SCOPE_PROFILE(jleGame::UpdateActiveScenes)
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

std::vector<std::shared_ptr<jleScene>>& jleGame::GetActiveScenesRef() {
    return mActiveScenes;
}

bool jleGame::CheckSceneIsActive(const std::string& sceneName) {
    for (auto&& scene : mActiveScenes) {
        if (sceneName == scene->mSceneName) {
            return true;
        }
    }

    return false;
}
