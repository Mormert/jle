// Copyright (c) 2023. Johan Lind

#include "jleObject.h"

template <class Archive>
void
jleScene::serialize(Archive &archive)
{
    archive(CEREAL_NVP(sceneName), CEREAL_NVP(_sceneObjects), CEREAL_NVP(_objectsInstantiatedCounter));

    for (auto &&object : _sceneObjects) {
        object->propagateOwnedByScene(this);
    }
}

template <typename T>
inline std::shared_ptr<T> jleScene::spawnObject() {
    static_assert(std::is_base_of<jleObject, T>::value,
                  "T must derive from jleObject");

    std::shared_ptr<T> newSceneObject = std::make_shared<T>();
    configurateSpawnedObject(newSceneObject);

    return newSceneObject;
}

inline std::shared_ptr<jleObject> jleScene::spawnObject(
    const std::string &objName) {
    auto newSceneObject =
        jleTypeReflectionUtils::instantiateObjectByString(objName);
    configurateSpawnedObject(newSceneObject);

    return newSceneObject;
}

inline std::vector<std::shared_ptr<jleObject>> &jleScene::sceneObjects() {
    return _sceneObjects;
}
