// Copyright (c) 2022. Johan Lind

#include "jleObject.h"

template <typename T>
inline std::shared_ptr<T> jleScene::SpawnObject() {
    static_assert(std::is_base_of<jleObject, T>::value,
                  "T must derive from jleObject");

    std::shared_ptr<T> newSceneObject = std::make_shared<T>();
    ConfigurateSpawnedObject(newSceneObject);

    return newSceneObject;
}

inline std::shared_ptr<jleObject> jleScene::SpawnObject(
    const std::string& objName) {
    auto newSceneObject =
        jleTypeReflectionUtils::InstantiateObjectByString(objName);
    ConfigurateSpawnedObject(newSceneObject);

    return newSceneObject;
}

inline std::vector<std::shared_ptr<jleObject>>& jleScene::GetSceneObjects() {
    return _sceneObjects;
}
