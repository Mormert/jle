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

#ifndef JLE_OBJECT
#define JLE_OBJECT

#include "jleCommon.h"

#include "jlePath.h"
#include "jleSerializedResource.h"
#include "jleTransform.h"
#include "jleTypeReflectionUtils.h"

#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>

#include <memory>
#include <optional>
#include <vector>

class jleScene;
class jleSceneClient;
class jleSceneServer;

enum class jleObjectNetworkType : uint8_t { REGULAR, CLIENT, SERVER };

class jleObject : public jleSerializedOnlyResource, public std::enable_shared_from_this<jleObject>
{
    JLE_REGISTER_OBJECT_TYPE(jleObject)
    JLE_REGISTER_RESOURCE_TYPE(jleObject, "jobj")
public:

    std::shared_ptr<jleObject> duplicate(bool childChain = false);

    std::shared_ptr<jleObject> duplicateTemplate(bool childChain = false);

    template <class Archive>
    void serialize(Archive &archive);

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedOnlyResource)

    jleObject();

    ~jleObject() override = default;

    template <typename T>
    std::shared_ptr<T> addComponent();

    template <typename T>
    void addComponent(const std::shared_ptr<T> &component);

    std::shared_ptr<jleComponent> addComponentByName(const std::string &component_name);

    template <typename T>
    std::shared_ptr<T> getComponent();

    // If found, returns first component found of given type from this object, or one of its children
    template <typename T>
    std::shared_ptr<T> getComponentInChildren(jleObject *object = nullptr);

    template <typename T>
    std::shared_ptr<T> spawnChildObject();

    std::shared_ptr<jleObject> spawnChildObjectFromTemplate(const jlePath& path);

    std::shared_ptr<jleObject> spawnChildObject(const std::string &objName);

    void saveAsObjectTemplate();

    // Called from components
    void destroyComponent(jleComponent *component);

    void destroyComponentAtIndex(uint32_t index);

    void destroyObject();

    int componentCount();

    std::vector<std::shared_ptr<jleComponent>> &components();

    void attachChildObject(const std::shared_ptr<jleObject> &object);

    void detachObjectFromParent();

    std::vector<std::shared_ptr<jleObject>> &childObjects();

    void tryFindChildWithInstanceId(int instanceId, std::shared_ptr<jleObject> &outObject);

    jleObject *parent();

    [[nodiscard]] std::weak_ptr<jleObject> weakPtrToThis();

    // If this object is based on a template
    std::optional<jlePath> __templatePath{};

    void setInstanceName(const std::string &name);

    const std::string &instanceName() const;

    uint32_t instanceID() const;

    jleObjectNetworkType networkObjectType();

    int32_t netID() const;

    int32_t netOwnerID() const;

    jleTransform &getTransform();

    bool pendingKill();

private:
    friend class jleTransform;
    friend class jleScene;
    friend class jleSceneServer;
    friend class jleSceneClient;

    explicit jleObject(jleScene *scene);

    void propagateDestroy();

    void propagateOwnedByScene(jleScene *scene);
    void propagateOwnedBySceneClient(jleSceneClient *scene);
    void propagateOwnedBySceneServer(jleSceneServer *scene);

    void replaceChildrenWithTemplate();

    void startComponents();

    void updateComponents(float dt);

    void updateComponentsEditor(float dt);

    void updateComponentsServer(float dt);

    void updateChildren(float dt);

    void updateChildrenEditor(float dt);

    void updateChildrenServer(float dt);

    void addComponentStart(const std::shared_ptr<jleComponent>& c);

    std::string _instanceName;

    bool _pendingKill = false;

    bool _isStarted = false;

    uint32_t _instanceID{};

    int32_t _netId{-1};
    int32_t _networkOwnerID{-1};

protected:
    friend class jleGame;
    friend class jleLuaEnvironment;
    std::vector<std::shared_ptr<jleComponent>> _components{};

    jleTransform _transform;

    std::vector<std::shared_ptr<jleObject>> __childObjects{};

    jleObject *_parentObject = nullptr;

    jleScene *_containedInScene = nullptr;

    jleSceneClient *_containedInSceneClient = nullptr; // is unset if not in a "client scene"
    jleSceneServer *_containedInSceneServer = nullptr; // is unset if not in a "server scene"

    static inline uint32_t _instanceIdCounter{0};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleObject)

CEREAL_REGISTER_TYPE(jleObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleObject)

#include "jleObject.inl"

#endif