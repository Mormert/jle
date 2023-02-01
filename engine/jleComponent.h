// Copyright (c) 2022. Johan Lind

#ifndef JLE_COMPONENT
#define JLE_COMPONENT

#include "jleJson.h"
#include "jleTypeReflectionUtils.h"

#include "json.hpp"

#include <string_view>

#include <cereal/archives/json.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>

class jleScene;
class jleComponent;
class jleTransform;

class jleComponent
{
public:
    jleComponent(jleObject *owner = nullptr, jleScene *scene = nullptr);

    virtual ~jleComponent() = default;

    template <class Archive>
    void
    serialize(Archive &archive)
    {
    }

    virtual void
    start()
    {
    }

    virtual void
    update(float dt)
    {
    }

    [[maybe_unused]] virtual void
    editorUpdate(float dt)
    {
    }

    void destroy();

    virtual const std::string_view componentName() const = 0;

    virtual void
    toJson(nlohmann::json &j_out)
    {
    }

    virtual void
    fromJson(const nlohmann::json &j_in)
    {
    }

    jleTransform& getTransform();

    friend void to_json(nlohmann::json &j, const std::shared_ptr<jleComponent> c);

    friend void from_json(const nlohmann::json &j, std::shared_ptr<jleComponent> &c);

    template <typename T>
    [[nodiscard]] std::shared_ptr<T> addDependencyComponentInStart();

protected:
    friend class jleObject;
    friend class jleScene;

    // The object that owns this component
    jleObject *_attachedToObject;

    // The scene in which this component's object lives
    jleScene *_containedInScene;
};

void to_json(nlohmann::json &j, const std::shared_ptr<jleComponent> c);

void from_json(const nlohmann::json &j, std::shared_ptr<jleComponent> &c);

#include "jleComponent.inl"

#endif