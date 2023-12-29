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

#include "jleSpritesheet.h"
#include "jleGameEngine.h"
#include "jleResource.h"
#include <filesystem>
#include <fstream>

void
jleSpritesheet::loadImage()
{
    std::string pngPath = _pathJson.substr(0, _pathJson.find(".", 0)) + ".png";

    _imageTexture = gEngine->resources().loadResourceFromFile<jleTexture>(jlePath{pngPath});
}

bool
jleSpritesheet::loadFromFile(const jlePath &path)
{
    _pathJson = path.getRealPath();
    std::ifstream i(path.getRealPath());
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        from_json(j["frames"], *this);
        loadImage();
        return true;
    } else {
        LOG_ERROR << "Could not load Spritesheet json file " << path.getRealPath();
        return false;
    }
}

jleSpritesheet::jleSpritesheet(const jlePath &path)
{
    _pathJson = path.getRealPath();
    loadFromFile(path);
}
