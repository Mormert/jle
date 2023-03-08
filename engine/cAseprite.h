// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleAseprite.h"
#include "jleComponent.h"
#include "jleTransform.h"

class cAseprite : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cAseprite)
public:
    explicit cAseprite(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_asepritePaths),
           CEREAL_NVP(_width),
           CEREAL_NVP(_height),
           CEREAL_NVP(_textureX),
           CEREAL_NVP(_textureY),
           CEREAL_NVP(_offsetX),
           CEREAL_NVP(_offsetY),
           CEREAL_NVP(_animating));
    }

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

    int addAsepritePath(const std::string &path);

    void currentAseprite(unsigned int index);

    void currentAsepriteFrame(unsigned int index);

    [[nodiscard]] unsigned int currentAsepriteIndex() const;

    [[nodiscard]] jleAseprite &activeAsepriteRef();

    std::vector<jleJsonString> _asepritePaths;

    uint32_t _width{}, _height{}, _textureX{}, _textureY{};
    int32_t _offsetX{}, _offsetY{};

    bool _animating = false;

private:
    unsigned int _currentlyActiveAseprite{0};
    std::vector<std::shared_ptr<jleAseprite>> _aseprites{};

    uint32_t _currentFrame = 0;
    uint32_t _currentFrameDurationMs{};
    float _currentFrameTimeSpent = 0.f;
};

CEREAL_REGISTER_TYPE(cAseprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cAseprite)
