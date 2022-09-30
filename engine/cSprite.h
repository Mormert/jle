// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleComponent.h"

#include "jleQuadRenderingInterface.h"
#include "jleTexture.h"

#include <memory>
#include <string>

class cSprite : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cSprite)
public:
    cSprite(jleObject *owner = nullptr, jleScene *scene = nullptr);

    void CreateAndSetTextureFromPath(const std::string& path);

    void SetTexture(std::shared_ptr<jleTexture> texture);

    void SetRectangleDimensions(int width, int height);

    void SetTextureBeginCoordinates(int x, int y);

    virtual void Start() override;

    virtual void Update(float dt) override;

    virtual void ToJson(nlohmann::json& j_out) override;

    virtual void FromJson(const nlohmann::json& j_in) override;

private:
    std::string texturePath = "";

    TexturedQuad quad;
    std::shared_ptr<cTransform> transform;
};
