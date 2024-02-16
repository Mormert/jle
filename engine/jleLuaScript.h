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

class jleLuaScript : public jleResourceInterface, public std::enable_shared_from_this<jleLuaScript>
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleLuaScript, "lua");

    [[nodiscard]] bool loadFromFile(const jlePath &path) override;

    virtual void loadScript();

    void saveToFile() override;

protected:
    std::string _luaScriptName;
    std::string _sourceCode;
    std::shared_ptr<jleLuaEnvironment> _luaEnvironment;
    bool faultyState = true;
};

CEREAL_REGISTER_TYPE(jleLuaScript)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleResourceInterface, jleLuaScript)

#endif // JLE_LUASCRIPT_H
