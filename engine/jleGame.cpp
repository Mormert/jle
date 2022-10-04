// Copyright (c) 2022. Johan Lind

#include "jleGame.h"

void jleGame::updateActiveScenes(float dt) {
    JLE_SCOPE_PROFILE(jleGame::updateActiveScenes)
    for (int i = _activeScenes.size() - 1; i >= 0; i--) {
        if (_activeScenes[i]->bPendingSceneDestruction) {
            _activeScenes.erase(_activeScenes.begin() + i);
            continue;
        }

        _activeScenes[i]->sceneUpdate();
        _activeScenes[i]->processNewSceneObjects();
        _activeScenes[i]->updateSceneObjects(dt);
    }
}

std::vector<std::shared_ptr<jleScene>>& jleGame::activeScenesRef() {
    return _activeScenes;
}

bool jleGame::checkSceneIsActive(const std::string& sceneName) {
    for (auto&& scene : _activeScenes) {
        if (sceneName == scene->_sceneName) {
            return true;
        }
    }

    return false;
}
