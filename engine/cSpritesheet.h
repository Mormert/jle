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

#include "jleBuildConfig.h"

#include "jleComponent.h"
#include "jleSpritesheet.h"
#include "jleTransform.h"

class cSpritesheet : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cSpritesheet)
public:
    explicit cSpritesheet(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_spritesheetPath), CEREAL_NVP(_spriteName), CEREAL_NVP(_offset.x), CEREAL_NVP(_offset.y));
        // TODO: _spritesheet needs to be loaded based on _spritesheetPath...
    }

    void start() override;

    void update(float dt) override;

    void entity(const std::string &entityName);

protected:
    std::shared_ptr<jleSpritesheet> _spritesheet;

    bool _hasEntity = false;
    jleSpritesheetEntity _spritesheetEntityCache;

    std::string _spritesheetPath;
    std::string _spriteName;
    glm::vec2 _offset{};
};

CEREAL_REGISTER_TYPE(cSpritesheet)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cSpritesheet)
