// Copyright (c) 2023. Johan Lind

#ifndef JLE_OBJECT
#define JLE_OBJECT

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

class jleObject : public jleSerializedOnlyResource, public std::enable_shared_from_this<jleObject>
{
    JLE_REGISTER_OBJECT_TYPE(jleObject)
    JLE_REGISTER_RESOURCE_TYPE(jleObject, "jobj")
public:
    void
    saveToFile() override
    {
        if (__templatePath.has_value()) {
            std::ofstream save{__templatePath->getRealPath()};
            cereal::JSONOutputArchive outputArchive(save);
            outputArchive(shared_from_this());
        } else {
            LOGE << "Can't save an object that doesn't have a template path set!";
        }
    };

    std::shared_ptr<jleObject> duplicate(bool childChain = false);

    std::shared_ptr<jleObject> duplicateTemplate(bool childChain = false);

    template <class Archive>
    void serialize(Archive &archive);

    jleObject();

    ~jleObject() = default;

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
    std::shared_ptr<T> addDependencyComponent(const jleComponent *component);

    template <typename T>
    std::shared_ptr<T> spawnChildObject();

    std::shared_ptr<jleObject> spawnChildObject(const std::string &objName);

    void saveAsObjectTemplate();

    // Called from components
    void destroyComponent(jleComponent *component);

    void destroyObject();

    int componentCount();

    std::vector<std::shared_ptr<jleComponent>> &customComponents();

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

    int64_t netEntityID() const;

    int64_t netOwnerID() const;

    jleTransform &getTransform();

    bool pendingKill();

private:
    friend class jleScene;
    friend class jleSceneServer;
    friend class jleSceneClient;

    explicit jleObject(jleScene *scene);

    void propagateDestroy();

    void propagateOwnedByScene(jleScene *scene);

    void replaceChildrenWithTemplate();

    void startComponents();

    void updateComponents(float dt);

    void updateComponentsEditor(float dt);

    void updateComponentsServer(float dt);

    void updateChildren(float dt);

    void updateChildrenEditor(float dt);

    void updateChildrenServer(float dt);

    void addComponentStart(jleComponent *c);

    std::string _instanceName;

    bool _pendingKill = false;

    bool _isStarted = false;

    uint32_t _instanceID{};
    int64_t _networkEntityID{-1};
    int64_t _networkOwnerID{-1};

protected:
    friend class jleGame;
    friend class jleLuaEnvironment;
    std::vector<std::shared_ptr<jleComponent>> _components{};

    jleTransform _transform;

    std::vector<std::shared_ptr<jleObject>> __childObjects{};

    jleObject *_parentObject = nullptr;

    jleScene *_containedInScene = nullptr;

    static inline uint32_t _instanceIdCounter{0};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleObject)

CEREAL_REGISTER_TYPE(jleObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleObject)

#include "jleObject.inl"

#endif