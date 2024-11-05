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
#include "core/jleResourceHolder.h"
#include "jleGameEngine.h"
#include <fstream>
#include <plog/Log.h>

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
