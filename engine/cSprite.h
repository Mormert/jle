// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleComponent.h"

#include "jleQuads.h"
#include "jleTexture.h"

#include <memory>
#include <string>

class cSprite : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cSprite)
public:
    cSprite(jleObject *owner = nullptr, jleScene *scene = nullptr);

    void createAndSetTextureFromPath(const std::string& path);

    void texture(std::shared_ptr<jleTexture> texture);

    void rectangleDimensions(int width, int height);

    void textureBeginCoordinates(int x, int y);

    virtual void start() override;

    virtual void Update(float dt) override;

    virtual void toJson(nlohmann::json& j_out) override;

    virtual void fromJson(const nlohmann::json& j_in) override;

private:
    std::string texturePath = "";

    texturedQuad quad;
    std::shared_ptr<cTransform> transform;
};
