/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#ifndef JLE_COMPONENT
#define JLE_COMPONENT

#include "jleTypeReflectionUtils.h"

#ifdef JLE_BUILD_EDITOR
#include "editor/jleImGuiCerealArchive.h"
#endif
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>

#define SOL_ALL_SAFETIES_ON 1
#include <sol2/sol.hpp>

#include <string_view>

class jleScene;
class jleSceneClient;
class jleSceneServer;
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
    netSyncOut(cereal::BinaryOutputArchive &ar)
    {
    }

    virtual void
    netSyncIn(cereal::BinaryInputArchive &ar)
    {
    }

    virtual void
    start()
    {
    }

    virtual void
    serverStart()
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

    void syncServerToClient();

    void destroy();

    bool isDestroyed();

    virtual const std::string_view componentName() const = 0;

    jleTransform &getTransform();

    jleObject *object();

    std::shared_ptr<jleObject> getObjectSharedPtr();

    jleScene *scene();
    jleSceneServer *sceneServer();
    jleSceneClient *sceneClient();

protected:
    friend class jleObject;
    friend class jleScene;

    // The object that owns this component
    jleObject *_attachedToObject{};

    // The scene in which this component's object lives
    jleScene *_containedInScene{};
    jleSceneClient *_containedInSceneClient{};
    jleSceneServer *_containedInSceneServer{};

    bool _isDestroyed{false};

    // Should be enabled in component's constructor
    void enableParallelUpdate(int batchSize = 4);

private:
    bool _enableParallelUpdate{false};
    int _parallelUpdateBatchSize = 4;

    bool _isStarted{false};
};

#define NET_SYNC(...)                                                                                                  \
    void netSyncOut(cereal::BinaryOutputArchive &ar) override { ar(__VA_ARGS__); }                                     \
    void netSyncIn(cereal::BinaryInputArchive &ar) override { ar(__VA_ARGS__); }

CEREAL_REGISTER_TYPE(jleComponent)

#include "jleComponent.inl"

#endif