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

#pragma once

#include "jlECS/jlECS.h"
#include "core/jleImage.h"
#include "modules/jleGameModules.h"

namespace jlECS
{
class ECS;
}

class jleImageModule : public jleGameBaseModule
{
public:
    virtual void initializeECS(jlECS::ECS &ecs);

    void loadImages(jlECS::ECS &ecs);

    [[nodiscard]] std::shared_ptr<jleImage> getLoadedImage(const jlePath &path) const;
    std::shared_ptr<jleImage> loadImageSync(const jlePath &path);
    void loadImageAsync(const jlePath &path);

    // Allows for inserting images that are generated at runtime
    void insertRuntimeImage(const std::shared_ptr<jleImage> &image, const jlePath &path);

private:
    std::unordered_set<jlePath> _imagePathsToLoad;
    std::unordered_map<jlePath, std::shared_ptr<jleImage>> _loadedImages;
};
