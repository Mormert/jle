// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleJson.h"
#include "jleTypeReflectionUtils.h"

#include "3rdparty/json.hpp"

#include <string_view>

class jleScene;

class jleComponent : public jleJsonInterface<nlohmann::json> {
public:
    jleComponent(jleObject *owner = nullptr, jleScene *scene = nullptr);

    virtual void Start() {}

    virtual void Update(float dt) {}

    [[maybe_unused]] virtual void EditorUpdate(float dt) {}

    void Destroy();

    virtual const std::string_view GetComponentName() const = 0;

    virtual void ToJson(nlohmann::json& j_out) {}

    virtual void FromJson(const nlohmann::json& j_in) {}

    friend void to_json(nlohmann::json& j,
                        const std::shared_ptr<jleComponent> c);

    friend void from_json(const nlohmann::json& j,
                          std::shared_ptr<jleComponent>& c);

protected:
    friend class jleObject;

    // The object that owns this component
    jleObject *mAttachedToObject;

    // The scene in which this component's object lives
    jleScene *mContainedInScene;
};

void to_json(nlohmann::json& j, const std::shared_ptr<jleComponent> c);

void from_json(const nlohmann::json& j, std::shared_ptr<jleComponent>& c);
