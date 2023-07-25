// Copyright (c) 2023. Johan Lind

#ifndef JLE_LUASCRIPTCOMPONENT_H
#define JLE_LUASCRIPTCOMPONENT_H

#include "jleLuaScript.h"
class jleLuaScriptComponent : public jleLuaScript, public std::enable_shared_from_this<jleLuaScriptComponent>
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleLuaScriptComponent, lua);

    void setupLua(sol::table &self, jleObject *ownerObject);

    void loadScript() override;

    void startLua(sol::table &self);
    void updateLua(sol::table &self, float dt);
    void onDestroyLua(sol::table &self);

private:
    sol::protected_function _setupLua;
    sol::protected_function _startLua;
    sol::protected_function _updateLua;
    sol::protected_function _onDestroyLua;

};

CEREAL_REGISTER_TYPE(jleLuaScriptComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleLuaScript, jleLuaScriptComponent)

#endif // JLE_LUASCRIPTCOMPONENT_H
