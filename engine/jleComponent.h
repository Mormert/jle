// Copyright (c) 2023. Johan Lind

#ifndef JLE_COMPONENT
#define JLE_COMPONENT

#include "jleTypeReflectionUtils.h"

#ifdef BUILD_EDITOR
#include "editor/jleImGuiCerealArchive.h"
#endif
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
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
    netSyncOut(cereal::JSONOutputArchive &ar)
    {
    }

    virtual void
    netSyncIn(cereal::JSONInputArchive &ar)
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

    virtual void
    parallelUpdate(float dt)
    {
        // Danger zone, enabled by enableParallelUpdate() in component constructor.
        // Called on all components of this type concurrently before recursive scene graph update().
    }

    int parallelUpdateBatchSize();

    bool parallelUpdateEnabled();

    [[maybe_unused]] virtual void
    editorUpdate(float dt)
    {
    }

    [[maybe_unused]] virtual void
    serverUpdate(float dt)
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

    bool isDestroyed();

    virtual const std::string_view componentName() const = 0;

    jleTransform &getTransform();

    template <typename T>
    [[nodiscard]] std::shared_ptr<T> addDependencyComponentInStart();

    jleObject *object();

    jleScene *scene();

protected:
    friend class jleObject;
    friend class jleScene;

    // The object that owns this component
    jleObject *_attachedToObject{};

    // The scene in which this component's object lives
    jleScene *_containedInScene{};

    bool _isDestroyed{false};

    // Should be enabled in component's constructor
    void enableParallelUpdate(int batchSize = 4);

private:
    bool _enableParallelUpdate{false};
    int _parallelUpdateBatchSize = 4;
};

#define NET_SYNC(...)                                                                                                  \
    void netSyncOut(cereal::JSONOutputArchive &ar) override { ar(__VA_ARGS__); }                                     \
    void netSyncIn(cereal::JSONInputArchive &ar) override { ar(__VA_ARGS__); }

CEREAL_REGISTER_TYPE(jleComponent)

#include "jleComponent.inl"

#endif