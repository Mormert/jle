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

#include "jleResourceInterface.h"
#include "jleTexture.h"

#include "json.hpp"

#include <string>
#include <unordered_map>

struct jleSpritesheetEntity {
    struct jleSpritesheetEntityFrame {
        int x, y, width, height;
    } frame;

    struct jleSpritesheetEntitySource {
        int x, y, width, height;
    } sourceSize;
};

struct jleSpritesheet : public jleResourceInterface {

    jleSpritesheet() = default;

    explicit jleSpritesheet(const jlePath &path);

    void loadImage();

    [[nodiscard]] bool loadFromFile(const jlePath &path) override;

    std::unordered_map<std::string, jleSpritesheetEntity> _spritesheetEntities;

    std::shared_ptr<jleTexture> _imageTexture;
    std::string _pathJson;
};

void from_json(const nlohmann::json &j, jleSpritesheet &s);

void from_json(const nlohmann::json &j, jleSpritesheetEntity &s);

void from_json(const nlohmann::json &j,
               jleSpritesheetEntity::jleSpritesheetEntityFrame &s);

void from_json(const nlohmann::json &j,
               jleSpritesheetEntity::jleSpritesheetEntitySource &s);
