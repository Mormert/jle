// Copyright (c) 2023. Johan Lind

#ifndef JLE_COMPONENT
#define JLE_COMPONENT

#include "jleTypeReflectionUtils.h"

#ifdef BUILD_EDITOR
#include "editor/jleImGuiCerealArchive.h"
#endif
#include <cereal/archives/json.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>

#define SOL_ALL_SAFETIES_ON 1
#include <sol2/sol.hpp>

#include <string_view>

class jleScene;
class jleComponent;
class jleTransform;

class jleComponent
{
public:
    explicit jleComponent(jleObject *owner, jleScene *scene);

    jleComponent(){};

    virtual ~jleComponent() = default;

    [[nodiscard]] virtual std::shared_ptr<jleComponent> clone() const = 0;

    virtual void registerSelfLua(sol::table &self) = 0;

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
    onDestroy()
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

    [[maybe_unused]] virtual void
    editorGizmosRender(bool selected)
    {
    }

    [[maybe_unused]] virtual void
    editorInspectorImGuiRender()
    {
    }

    virtual void
    registerLua(sol::state &lua, sol::table &table)
    {
    }

    void destroy();

    virtual const std::string_view componentName() const = 0;

    jleTransform &getTransform();

    template <typename T>
    [[nodiscard]] std::shared_ptr<T> addDependencyComponentInStart();

protected:
    friend class jleObject;
    friend class jleScene;

    // The object that owns this component
    jleObject *_attachedToObject{};

    // The scene in which this component's object lives
    jleScene *_containedInScene{};
};

CEREAL_REGISTER_TYPE(jleComponent)

#include "jleComponent.inl"

#endif