// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleComponent.h"

#include "jleQuads.h"
#include "jleTexture.h"

#include <memory>
#include <string>

class cSpriteDepth : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cSpriteDepth)
public:
    explicit cSpriteDepth(jleObject *owner = nullptr,
                          jleScene *scene = nullptr);

    void createAndSetTextureFromPath(const std::string& pathDiffuse,
                                     const std::string& pathHeight,
                                     const std::string& pathNormal);

    void rectangleDimensions(int width, int height);

    void textureBeginCoordinates(int x, int y);

    virtual void start() override;

    virtual void Update(float dt) override;

    virtual void toJson(nlohmann::json& j_out) override;

    virtual void fromJson(const nlohmann::json& j_in) override;

private:
    std::string texturePathDiffuse = "";
    std::string texturePathHeight = "";
    std::string texturePathNormal = "";

    texturedHeightQuad quad;
    std::shared_ptr<cTransform> transform;
};
