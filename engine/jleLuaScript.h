// Copyright (c) 2023. Johan Lind

#ifndef JLE_LUASCRIPT_H
#define JLE_LUASCRIPT_H

#include "jleGameEngine.h"
#include "jleResourceInterface.h"
#include "jleTypeReflectionUtils.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol2/sol.hpp>

class jleLuaScript : public jleResourceInterface, public std::enable_shared_from_this<jleLuaScript>
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleLuaScript, lua);

    jleLoadFromFileSuccessCode loadFromFile(const jlePath &path) override;

    void saveToFile() override;

    std::shared_ptr<sol::state> &setupLua(sol::table &self, jleObject* ownerObject);

    void startLua(sol::table &self);

    void updateLua(sol::table &self, float dt);

    void onDestroyLua(sol::table &self);

private:
    void loadScript();

    std::shared_ptr<sol::state> _currentGameLua{};

    sol::protected_function _setupLua;
    sol::protected_function _startLua;
    sol::protected_function _updateLua;
    sol::protected_function _onDestroyLua;

    std::string _sourceCode;
    std::string _luaScriptName;

    bool faultyState = true;
};

CEREAL_REGISTER_TYPE(jleLuaScript)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleResourceInterface, jleLuaScript)

#endif // JLELUASCRIPT_H
