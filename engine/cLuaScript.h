// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleComponent.h"
#include "jleLuaScript.h"
#include "jleResourceRef.h"

class cLuaScript : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cLuaScript)
public:
    explicit cLuaScript(jleObject *owner = nullptr, jleScene *scene = nullptr);

    virtual ~cLuaScript();

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar( CEREAL_NVP(_scriptRef));
    }

    void start() override;

    void update(float dt) override;

    void onDestroy() override;

    sol::table& getSelf();

    bool runUpdate = true;

private:
    jleResourceRef<jleLuaScript> _scriptRef;

    std::shared_ptr<sol::state> _luaKeepAliveRef{};
    sol::table _self;

    std::function<void(sol::table, float)> _updateLua;
    std::function<void(sol::table)> _onDestroyLua;
};

CEREAL_REGISTER_TYPE(cLuaScript)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cLuaScript)
