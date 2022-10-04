// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleJson.h"
#include "jleTypeReflectionUtils.h"

#include "json.hpp"

#include <string_view>

class jleScene;

class jleComponent : public jleJsonInterface<nlohmann::json> {
public:
    jleComponent(jleObject *owner = nullptr, jleScene *scene = nullptr);

    virtual void start() {}

    virtual void Update(float dt) {}

    [[maybe_unused]] virtual void editorUpdate(float dt) {}

    void destroy();

    virtual const std::string_view componentName() const = 0;

    virtual void toJson(nlohmann::json& j_out) {}

    virtual void fromJson(const nlohmann::json& j_in) {}

    friend void to_json(nlohmann::json& j,
                        const std::shared_ptr<jleComponent> c);

    friend void from_json(const nlohmann::json& j,
                          std::shared_ptr<jleComponent>& c);

protected:
    friend class jleObject;

    // The object that owns this component
    jleObject *_attachedToObject;

    // The scene in which this component's object lives
    jleScene *_containedInScene;
};

void to_json(nlohmann::json& j, const std::shared_ptr<jleComponent> c);

void from_json(const nlohmann::json& j, std::shared_ptr<jleComponent>& c);
