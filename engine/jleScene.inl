/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleGameEngine.h"
#include "jleObject.h"
#include "jleResource.h"
#include <fstream>
#include <plog/Log.h>

template <class Archive>
void
jleScene::serialize(Archive &archive)
{
    archive(CEREAL_NVP(sceneName), CEREAL_NVP(_sceneObjects));

    for (auto &&object : _sceneObjects) {
        // Replace object with template object, if it is based on one
        if (object->__templatePath.has_value()) {
            auto path = object->__templatePath;
            try {
                auto original = gEngine->resources().loadResourceFromFile<jleObject>(object->__templatePath.value());

                auto copy = original->duplicateTemplate();
                object = copy;

                object->__templatePath = path;

            } catch (std::exception &e) {
                LOGE << "Failed to load object template: " << e.what();
            }
        }

        object->replaceChildrenWithTemplate();

        object->propagateOwnedByScene(this);
    }
}

template <typename T>
inline std::shared_ptr<T>
jleScene::spawnObject()
{
    static_assert(std::is_base_of<jleObject, T>::value, "T must derive from jleObject");

    std::shared_ptr<T> newSceneObject = std::make_shared<T>();
    setupObject(newSceneObject);

    return newSceneObject;
}

inline std::shared_ptr<jleObject>
jleScene::spawnObjectTypeByName(const std::string &objName)
{
    auto newSceneObject = jleTypeReflectionUtils::instantiateObjectByString(objName);
    setupObject(newSceneObject);

    return newSceneObject;
}

inline std::vector<std::shared_ptr<jleObject>> &
jleScene::sceneObjects()
{
    return _sceneObjects;
}
