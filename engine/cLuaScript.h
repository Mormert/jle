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
#include "jleLuaClassSerialization.h"
#include "jleLuaScript.h"
#include "jleResourceRef.h"

class cLuaScript : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cLuaScript)
public:
    template <class Archive>
    void serialize(Archive &ar);

    void start() override;

    void update(float dt) override;

    void onDestroy() override;

    sol::table &getSelf();

private:
    void initializeLuaComponent();
    bool _isInitialized{false};

    jleLuaClassSerialization _luaClass;

    sol::table _self;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cLuaScript)

CEREAL_REGISTER_TYPE(cLuaScript)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cLuaScript)
