// Copyright (c) 2022. Johan Lind

#pragma once

#include "cTransform.h"
#include "jleAseprite.h"
#include "jleComponent.h"

class cAseprite : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cAseprite)
public:
    explicit cAseprite(jleObject *owner = nullptr, jleScene *scene = nullptr);

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

    std::shared_ptr<cTransform> _transform{nullptr};

    uint32_t _currentFrame = 0;
    uint32_t _currentFrameDurationMs{};
    float _currentFrameTimeSpent = 0.f;
};
