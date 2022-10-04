// Copyright (c) 2022. Johan Lind

#include "jleObject.h"

template <typename T>
inline std::shared_ptr<T> jleScene::spawnObject() {
    static_assert(std::is_base_of<jleObject, T>::value,
                  "T must derive from jleObject");

    std::shared_ptr<T> newSceneObject = std::make_shared<T>();
    configurateSpawnedObject(newSceneObject);

    return newSceneObject;
}

inline std::shared_ptr<jleObject> jleScene::spawnObject(
    const std::string& objName) {
    auto newSceneObject =
        jleTypeReflectionUtils::instantiateObjectByString(objName);
    configurateSpawnedObject(newSceneObject);

    return newSceneObject;
}

inline std::vector<std::shared_ptr<jleObject>>& jleScene::sceneObjects() {
    return _sceneObjects;
}
