// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleComponent.h"

#include "jleQuadRenderingInterface.h"
#include "jleTexture.h"

#include <memory>
#include <string>

class cSpriteDepth : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cSpriteDepth)
public:
    explicit cSpriteDepth(jleObject *owner = nullptr,
                          jleScene *scene = nullptr);

    void CreateAndSetTextureFromPath(const std::string& pathDiffuse,
                                     const std::string& pathHeight,
                                     const std::string& pathNormal);

    void SetRectangleDimensions(int width, int height);

    void SetTextureBeginCoordinates(int x, int y);

    virtual void Start() override;

    virtual void Update(float dt) override;

    virtual void ToJson(nlohmann::json& j_out) override;

    virtual void FromJson(const nlohmann::json& j_in) override;

private:
    std::string texturePathDiffuse = "";
    std::string texturePathHeight = "";
    std::string texturePathNormal = "";

    TexturedHeightQuad quad;
    std::shared_ptr<cTransform> transform;
};
