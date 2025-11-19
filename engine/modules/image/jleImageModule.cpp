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

#include "jleImageModule.h"

#include <jlECS/jlECS.h>
#include <plog/Log.h>

void
jleImageModule::initializeECS(jlECS::ECS &ecs)
{
}

void
jleImageModule::loadImages(jlECS::ECS &ecs)
{
    // TODO: Move to task threads
    for (auto &path : _imagePathsToLoad) {
        loadImageSync(path);
    }
    _imagePathsToLoad.clear();
}

std::shared_ptr<jleImage>
jleImageModule::getLoadedImage(const jlePath &path) const
{
    if (const auto it = _loadedImages.find(path); it != _loadedImages.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<jleImage>
jleImageModule::loadImageSync(const jlePath &path)
{
    if (const auto it = _loadedImages.find(path); it != _loadedImages.end()) {
        return it->second;
    }

    auto imageToLoad = std::make_shared<jleImage>(path);
    if (imageToLoad->data()) {
        _loadedImages[path] = imageToLoad;
        imageToLoad->path = path;
        return _loadedImages[path];
    }

    LOGE << "Failed to load image from path: " << path.getVirtualPath().str();

    return nullptr;
}

void
jleImageModule::loadImageAsync(const jlePath &path)
{
    if (_loadedImages.contains(path)) {
        return;
    }
    _imagePathsToLoad.insert(path);
}

void
jleImageModule::insertRuntimeImage(const std::shared_ptr<jleImage> &image, const jlePath &path)
{
    _loadedImages.insert(std::make_pair(path, image));
}
