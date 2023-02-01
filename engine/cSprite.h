// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleComponent.h"
#include "jleTransform.h"

#include "jleQuads.h"
#include "jleTexture.h"

#include <memory>
#include <string>

class cSprite : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cSprite)
public:
    explicit cSprite(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(texturePath),
           CEREAL_NVP(quad.x),
           CEREAL_NVP(quad.y),
           CEREAL_NVP(quad.depth),
           CEREAL_NVP(quad.height),
           CEREAL_NVP(quad.width),
           CEREAL_NVP(quad.textureX),
           CEREAL_NVP(quad.textureY));
    }

    void createAndSetTextureFromPath(const std::string &path);

    void texture(std::shared_ptr<jleTexture> texture);

    void rectangleDimensions(int width, int height);

    void textureBeginCoordinates(int x, int y);

    virtual void start() override;

    virtual void update(float dt) override;

    virtual void toJson(nlohmann::json &j_out) override;

    virtual void fromJson(const nlohmann::json &j_in) override;

private:
    std::string texturePath = "";

    texturedQuad quad;
};

CEREAL_REGISTER_TYPE(cSprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cSprite)
