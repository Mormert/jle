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

#include "core/jleObject.h"
#include "core/jleResource.h"
#include "jleGameEngine.h"
#include <fstream>
#include <plog/Log.h>

template <class Archive>
void
jleScene::serialize(Archive &archive)
{
    jleSerializationContext &ctx = archive.ctx;

    archive(CEREAL_NVP(sceneName), CEREAL_NVP(_sceneObjects));

    for (auto &&object : _sceneObjects) {
        // Replace object with template object, if it is based on one
        if (object->__templatePath.has_value()) {
            auto path = object->__templatePath;
            try {

                auto original = ctx.resources->loadResourceFromFileT<jleObject>(object->__templatePath.value(), ctx);

                auto copy = original->duplicateTemplate();
                object = copy;

                object->__templatePath = path;

            } catch (std::exception &e) {
                LOGE << "Failed to load object template: " << e.what();
            }
        }

        object->replaceChildrenWithTemplate(ctx);

        object->propagateOwnedByScene(this);
    }
}

template <typename T>
inline std::shared_ptr<T>
jleScene::spawnObject(jleSerializationContext& ctx)
{
    static_assert(std::is_base_of<jleObject, T>::value, "T must derive from jleObject");

    std::shared_ptr<T> newSceneObject = std::make_shared<T>();
    setupObject(newSceneObject, ctx);

    return newSceneObject;
}

inline std::vector<std::shared_ptr<jleObject>> &
jleScene::sceneObjects()
{
    return _sceneObjects;
}
