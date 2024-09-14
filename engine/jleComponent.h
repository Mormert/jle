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

#include "core/jleCommon.h"

#include "jleTypeReflectionUtils.h"

#include "editor/jleImGuiArchive.h"
#include "serialization/jleBinaryArchive.h"
#include "serialization/jleJSONArchive.h"
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
class jleEditorModulesContext;
class jleEditorGizmos;

class jleComponent
{
public:
    virtual ~jleComponent() = default;

    [[nodiscard]] virtual std::shared_ptr<jleComponent> clone() const = 0;

    template <class Archive>
    void
    serialize(Archive &archive)
    {
    }

    virtual void
    netSyncOut(jleBinaryOutputArchive &ar)
    {
    }

    virtual void
    netSyncIn(jleBinaryInputArchive &ar)
    {
    }

    virtual void
    start(jleEngineModulesContext& ctx)
    {
    }

    virtual void
    serverStart(jleEngineModulesContext& ctx)
    {
    }

    virtual void
    onDestroy(jleEngineModulesContext& ctx)
    {
    }

    virtual void
    update(jleEngineModulesContext& ctx)
    {
    }

    virtual void
    parallelUpdate(jleEngineModulesContext& ctx)
    {
        // Danger zone, enabled by enableParallelUpdate() in component constructor.
        // Called on all components of this type concurrently before recursive scene graph update().
    }

    int parallelUpdateBatchSize();

    bool parallelUpdateEnabled();

    [[maybe_unused]] virtual void
    editorUpdate(jleEngineModulesContext& ctx)
    {
    }

    [[maybe_unused]] virtual void
    serverUpdate(jleEngineModulesContext& ctx)
    {
    }

    [[maybe_unused]] virtual void
    editorGizmosRender(jleFramePacket & renderGraph, jleEditorGizmos& gizmos)
    {
    }

    [[maybe_unused]] virtual void
    editorInspectorImGuiRender(jleEditorModulesContext& ctx)
    {
    }

    void syncServerToClient(jleSerializationContext& serializationContext);

    void destroy(jleEngineModulesContext& ctx);

    bool isDestroyed();

    [[nodiscard]] virtual const std::string_view componentName() const = 0;

    jleTransform &getTransform();

    jleObject *object();

    std::shared_ptr<jleObject> getObjectSharedPtr();

    jleScene *scene();
    jleSceneServer *sceneServer();
    jleSceneClient *sceneClient();

protected:
    friend class jleObject;
    friend class jleScene;
    friend class jleLuaEnvironment;

    virtual void
    registerLua(sol::state &lua)
    {
    }

    template <typename T>
    void registerLuaComponentFunctions_Impl(sol::usertype<jleObject>& luaObjType);

    virtual void registerLuaComponentFunctions(sol::usertype<jleObject>& luaObjType)
    {
    }

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
    void netSyncOut(jleBinaryOutputArchive &ar) override { ar(__VA_ARGS__); }                                     \
    void netSyncIn(jleBinaryInputArchive &ar) override { ar(__VA_ARGS__); }

CEREAL_REGISTER_TYPE(jleComponent)

#include "jleComponent.inl"

#endif