// Copyright (c) 2022. Johan Lind

#ifndef JLE_CSPRITESHEET_H
#define JLE_CSPRITESHEET_H

#include "cTransform.h"
#include "jleComponent.h"
#include "jleSpritesheet.h"

class cSpritesheet : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cSpritesheet)
public:
    explicit cSpritesheet(jleObject *owner = nullptr,
                          jleScene *scene = nullptr);

    void Start() override;

    void Update(float dt) override;

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

private:
    std::shared_ptr<cTransform> mTransform{nullptr};
    std::shared_ptr<jleSpritesheet> mSpritesheet;

    bool mHasEntity = false;
    jleSpritesheetEntity mSpritesheetEntityCache;

    std::string mSpritesheetPath;
    std::string mSpriteName;
};

#endif // JLE_CSPRITESHEET_H
