// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleComponent.h"
#include <sol2/sol.hpp>

class cLuaScript : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cLuaScript)
public:
    explicit cLuaScript(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_scriptPath));
    }

    void start() override;

    void update(float dt) override;

protected:
    jlePath _scriptPath;

    std::shared_ptr<sol::state> _lua;
};

CEREAL_REGISTER_TYPE(cLuaScript)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cLuaScript)
