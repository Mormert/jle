// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleComponent.h"
#include "jleLuaScriptComponent.h"
#include "jleResourceRef.h"

class cLuaScript : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cLuaScript)
public:
    explicit cLuaScript(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar( CEREAL_NVP(_scriptRef), CEREAL_NVP(_specializationScript));
    }

    void start() override;

    void update(float dt) override;

    void onDestroy() override;

    sol::table& getSelf();

    bool runUpdate = true;

    void editorInspectorImGuiRender() override;

private:
    jleResourceRef<jleLuaScriptComponent> _scriptRef;
    std::string _specializationScript = "local self = ...;\n";
    sol::table _self;

    std::function<void(sol::table, float)> _updateLua;
    std::function<void(sol::table)> _onDestroyLua;
};

CEREAL_REGISTER_TYPE(cLuaScript)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cLuaScript)
