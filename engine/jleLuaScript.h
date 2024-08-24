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

#ifndef JLE_LUASCRIPT_H
#define JLE_LUASCRIPT_H

#include "jleBuildConfig.h"

#include "jleGameEngine.h"
#include "jleResourceInterface.h"
#include "jleTypeReflectionUtils.h"

class jleLuaScript : public jleResourceInterface
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleLuaScript, "lua");

    [[nodiscard]] bool loadFromFile(const jlePath &path) override;

    void saveToFile(jleSerializationContext& ctx) override;

protected:

    friend class jleLuaEnvironment;
    void loadScriptIntoLuaEnv(jleLuaEnvironment& luaEnvironment);

    std::string _luaScriptName;
    std::string _sourceCode;

    bool _failsLoading = true;
};

CEREAL_REGISTER_TYPE(jleLuaScript)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleResourceInterface, jleLuaScript)

#endif // JLE_LUASCRIPT_H
