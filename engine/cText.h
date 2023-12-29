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

/*

#include "jleAseprite.h"
#include "jleComponent.h"
#include "jleFont.h"
#include "jleTransform.h"

class cText : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cText)
public:
    explicit cText(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_fontPath),
           CEREAL_NVP(_text),
           CEREAL_NVP(_fontSize),
           CEREAL_NVP(_colorR),
           CEREAL_NVP(_colorG),
           CEREAL_NVP(_colorB),
           CEREAL_NVP(_colorA));
        // TODO: Load _font
    }

    void start() override;

    void update(float dt) override;

    void text(const std::string &text);

private:
    std::shared_ptr<jleFont> _font{nullptr};

    std::string _fontPath;
    std::string _text;
    uint32_t _fontSize{16};
    float _colorR{1.f}, _colorG{1.f}, _colorB{1.f}, _colorA{1.f};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cText)


CEREAL_REGISTER_TYPE(cText)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cText)

 */