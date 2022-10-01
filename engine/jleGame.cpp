// Copyright (c) 2022. Johan Lind

#include "jleGame.h"

void jleGame::UpdateActiveScenes(float dt) {
    JLE_SCOPE_PROFILE(jleGame::UpdateActiveScenes)
    for (int i = _activeScenes.size() - 1; i >= 0; i--) {
        if (_activeScenes[i]->bPendingSceneDestruction) {
            _activeScenes.erase(_activeScenes.begin() + i);
            continue;
        }

        _activeScenes[i]->SceneUpdate();
        _activeScenes[i]->ProcessNewSceneObjects();
        _activeScenes[i]->UpdateSceneObjects(dt);
    }
}

std::vector<std::shared_ptr<jleScene>>& jleGame::GetActiveScenesRef() {
    return _activeScenes;
}

bool jleGame::CheckSceneIsActive(const std::string& sceneName) {
    for (auto&& scene : _activeScenes) {
        if (sceneName == scene->_sceneName) {
            return true;
        }
    }

    return false;
}
