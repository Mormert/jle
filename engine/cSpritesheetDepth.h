// Copyright (c) 2022. Johan Lind

#pragma once

#include "cSpritesheet.h"
#include "jleQuads.h"

class cSpritesheetDepth : public cSpritesheet
{
    JLE_REGISTER_COMPONENT_TYPE(cSpritesheetDepth)
public:
    explicit cSpritesheetDepth(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_spritesheetPathDepth), CEREAL_NVP(_spritesheetPathDiffuse), CEREAL_NVP(_spritesheetPathNormal));
    }

    void createAndSetTextureFromPath(const std::string &pathDiffuse,
                                     const std::string &pathHeight,
                                     const std::string &pathNormal);

    void update(float dt) override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

protected:
    std::string _spritesheetPathDiffuse;
    std::string _spritesheetPathDepth;
    std::string _spritesheetPathNormal;
    jleTexturedHeightQuad _quad;
};

CEREAL_REGISTER_TYPE(cSpritesheetDepth)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cSpritesheetDepth)
