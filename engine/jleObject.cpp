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

#include "jleObject.h"
#include "jleGame.h"
#include "jleGameEngine.h"
#include "jleNetworkEvent.h"
#include "jlePathDefines.h"
#include "jleScene.h"
#include "jleSceneClient.h"
#include "jleSceneServer.h"
#include "jleTransform.h"
#include "modules/scripting/components/cLuaScript.h"

#include <fstream>
#include <optional>

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleObject)

struct jleAddComponentEvent : public jleServerToClientEvent {
    void
    execute(jleEngineModulesContext& ctx) override
    {
        auto &scene = getSceneClient();
        if (auto object = scene.getObjectFromNetId(objectNetId)) {
            object->addComponent(component, ctx);
        }
    }

    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(objectNetId), CEREAL_NVP(component));
    }

    int32_t objectNetId{};
    std::shared_ptr<jleComponent> component;
};

JLE_REGISTER_NET_EVENT(jleAddComponentEvent)

struct jleDestroyComponentEvent : public jleServerToClientEvent {
    void
    execute(jleEngineModulesContext& ctx) override
    {
        auto &scene = getSceneClient();
        if (auto object = scene.getObjectFromNetId(objectNetId)) {
            object->destroyComponentAtIndex(componentIndex);
        }
    }

    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(objectNetId), CEREAL_NVP(componentIndex));
    }

    int32_t objectNetId{};
    int8_t componentIndex{};
};

JLE_REGISTER_NET_EVENT(jleDestroyComponentEvent)

struct jleAttachChildEvent : public jleServerToClientEvent {
    void
    execute(jleEngineModulesContext& ctx) override
    {
        auto &scene = getSceneClient();
        if (auto parent = scene.getObjectFromNetId(objectNetIdParent)) {
            if (auto child = scene.getObjectFromNetId(objectNetIdChild)) {
                parent->attachChildObject(child);
            }
        }
    }

    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(objectNetIdParent), CEREAL_NVP(objectNetIdChild));
    }

    int32_t objectNetIdParent{};
    int32_t objectNetIdChild{};
};

JLE_REGISTER_NET_EVENT(jleAttachChildEvent)

jleObject::
jleObject()
    : _transform{this}
{
    _instanceID = _instanceIdCounter++;
}

void
jleObject::destroyComponent(jleComponent *component, jleEngineModulesContext& ctx)
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        if (_components[i].get() == component) {
            if (!ctx.gameRuntime.isGameKilled()) {

                if (auto luaScriptComponent = getComponent<cLuaScript>()) {
                    luaScriptComponent->getSelf()[component->componentName()] = sol::lua_nil;
                }

                component->onDestroy(ctx);
            }
            component->_isDestroyed = true;
            _components.erase(_components.begin() + i);

            if (networkObjectType() == jleObjectNetworkType::SERVER) {
                auto event = jleMakeNetEvent<jleDestroyComponentEvent>();
                event->objectNetId = netID();
                event->componentIndex = static_cast<int8_t>(i);
                _containedInSceneServer->sendNetworkEventBroadcast(std::move(event));
            }
        }
    }
}

void
jleObject::destroyComponentAtIndex(uint32_t index)
{
    if (index > _components.size()) {
        LOGW << "Tried to destroy component out of bounds at index: " << index;
        return;
    }
    auto component = _components[index];
    component->_isDestroyed = true;
    _components.erase(_components.begin() + index);
}

void
jleObject::destroyObject()
{
    _pendingKill = true;
}

int
jleObject::componentCount()
{
    return _components.size();
}

std::vector<std::shared_ptr<jleComponent>> &
jleObject::components()
{
    return _components;
}

std::vector<std::shared_ptr<jleObject>> &
jleObject::childObjects()
{
    return __childObjects;
}

void
jleObject::attachChildObject(const std::shared_ptr<jleObject> &object)
{
    // The objects must live in the same scene
    jleAssert(object->_containedInScene == _containedInScene);
    if (object->_parentObject) {
        // Remove the object from previous parent, if any
        auto it = std::find(
            object->_parentObject->__childObjects.begin(), object->_parentObject->__childObjects.end(), object);
        object->_parentObject->__childObjects.erase(it);
    } else {
        // Else the object is directly in the scene
        // Remove the object from existing directly in the scene, and instead as
        // a node under this object
        if (object->_isStarted) {
            auto it =
                std::find(_containedInScene->_sceneObjects.begin(), _containedInScene->_sceneObjects.end(), object);
            if (it != _containedInScene->_sceneObjects.end()) {
                _containedInScene->_sceneObjects.erase(it);
            }
        }
    }

    object->_parentObject = this;
    __childObjects.push_back(object);

    if (networkObjectType() == jleObjectNetworkType::SERVER) {
        auto event = jleMakeNetEvent<jleAttachChildEvent>();
        event->objectNetIdParent = netID();
        event->objectNetIdChild = object->netID();
        _containedInSceneServer->sendNetworkEventBroadcast(std::move(event));
    }

    // if (auto t = object->component<cTransform>()) {
    // t->flagDirty();
    // }
}

void
jleObject::detachObjectFromParent()
{
    if (_parentObject) {
        int i = 0;
        std::shared_ptr<jleObject> thiz;
        for (auto &&o : _parentObject->__childObjects) {
            if (o.get() == this) {
                thiz = o;
                _parentObject->__childObjects.erase(_parentObject->__childObjects.begin() + i);
                break;
            }
            i++;
        }
        // Insert this object to be contained directly in the scene
        _containedInScene->_sceneObjects.push_back(thiz);
    }

    _parentObject = nullptr;
}

jleObject::
jleObject(jleScene *scene)
    : _containedInScene{scene}, _transform{this}
{
}

void
jleObject::startComponents(jleEngineModulesContext& ctx)
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        if (!_components[i]->_isStarted) {
            if (networkObjectType() == jleObjectNetworkType::SERVER) {
                _components[i]->serverStart(ctx);
            } else {
                _components[i]->start(ctx);
            }
            _components[i]->_isStarted = true;
        }
        if (_components[i]->_enableParallelUpdate) {
            ctx.gameRuntime.getGame().addParallelComponent(_components[i]);
        }
    }
}

void
jleObject::updateComponents(jleEngineModulesContext& ctx)
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        _components[i]->update(ctx);
    }
}

void
jleObject::updateComponentsEditor(jleEngineModulesContext& ctx)
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        _components[i]->editorUpdate(ctx);
    }
}

void
jleObject::updateComponentsServer(jleEngineModulesContext& ctx)
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        _components[i]->serverUpdate(ctx);
    }
}

void
jleObject::updateChildren(jleEngineModulesContext& ctx)
{
    for (int32_t i = __childObjects.size() - 1; i >= 0; i--) {
        if (__childObjects[i]->_pendingKill) {
            __childObjects[i]->propagateDestroy(ctx);
            __childObjects.erase(__childObjects.begin() + i);
            continue;
        }

        __childObjects[i]->updateComponents(ctx);

        // Recursively update children after this object has updated
        __childObjects[i]->updateChildren(ctx);
    }
}

void
jleObject::updateChildrenEditor(jleEngineModulesContext& ctx)
{
    for (int32_t i = __childObjects.size() - 1; i >= 0; i--) {
        if (__childObjects[i]->_pendingKill) {
            __childObjects.erase(__childObjects.begin() + i);
            continue;
        }

        __childObjects[i]->updateComponentsEditor(ctx);

        // Recursively update children after this object has updated
        __childObjects[i]->updateChildrenEditor(ctx);
    }
}

void
jleObject::updateChildrenServer(jleEngineModulesContext& ctx)
{
    for (int32_t i = __childObjects.size() - 1; i >= 0; i--) {
        if (__childObjects[i]->_pendingKill) {
            __childObjects.erase(__childObjects.begin() + i);
            continue;
        }

        __childObjects[i]->updateComponentsServer(ctx);

        // Recursively update children after this object has updated
        __childObjects[i]->updateChildrenServer(ctx);
    }
}

jleObject *
jleObject::parent()
{
    return _parentObject;
}

std::shared_ptr<jleObject>
jleObject::duplicate(bool childChain)
{
    auto duplicated = clone();

    duplicated->_components.clear();
    duplicated->__childObjects.clear();
    duplicated->_transform._owner = duplicated.get();
    duplicated->_instanceID = _instanceIdCounter++;
    duplicated->_isStarted = false; // Note that duplicating an object will run its start function!

    for (auto &&component : _components) {
        auto clonedComponent = component->clone();
        clonedComponent->_containedInScene = _containedInScene;
        clonedComponent->_attachedToObject = duplicated.get();
        duplicated->_components.push_back(clonedComponent);
    }

    for (auto &&object : __childObjects) {
        auto duplicatedChild = object->duplicate(true);
        duplicatedChild->_parentObject = duplicated.get();
        duplicated->__childObjects.push_back(duplicatedChild);
    }

    _containedInScene->_newSceneObjects.push_back(duplicated);

    if (duplicated->parent() && !childChain) {
        duplicated->parent()->__childObjects.push_back(duplicated);
    }

    return duplicated;
}

std::shared_ptr<jleObject>
jleObject::duplicateTemplate(bool childChain)
{
    auto duplicated = clone();

    duplicated->_components.clear();
    duplicated->__childObjects.clear();
    duplicated->_instanceID = _instanceIdCounter++;
    duplicated->_transform._owner = duplicated.get();

    for (auto &&component : _components) {
        auto clonedComponent = component->clone();
        clonedComponent->_attachedToObject = duplicated.get();
        duplicated->_components.push_back(clonedComponent);
    }

    for (auto &&object : __childObjects) {
        auto duplicatedChild = object->duplicate(true);
        duplicatedChild->_parentObject = duplicated.get();
        duplicated->__childObjects.push_back(duplicatedChild);
    }

    if (duplicated->parent() && !childChain) {
        duplicated->parent()->__childObjects.push_back(duplicated);
    }

    return duplicated;
}

void
jleObject::saveAsObjectTemplate(jleSerializationContext& serializationContext)
{
    if (path.isEmpty()) {
        path = jlePath{"GR:otemps/" + _instanceName + ".jobj"};
    }
    saveToFile(serializationContext);
}

void
jleObject::setInstanceName(const std::string &name)
{
    _instanceName = name;
}

const std::string &
jleObject::instanceName() const
{
    return _instanceName;
}

uint32_t
jleObject::instanceID() const
{
    return _instanceID;
}

int32_t
jleObject::netID() const
{
    return _netId;
}

int32_t
jleObject::netOwnerID() const
{
    return _networkOwnerID;
}

void
jleObject::tryFindChildWithInstanceId(int instanceId, std::shared_ptr<jleObject> &outObject)
{
    if (instanceId == instanceID()) {
        outObject = std::static_pointer_cast<jleObject>(shared_from_this());
        return;
    }

    for (auto &&child : childObjects()) {
        child->tryFindChildWithInstanceId(instanceId, outObject);
    }
}

jleTransform &
jleObject::getTransform()
{
    return _transform;
}

void
jleObject::propagateOwnedByScene(jleScene *scene)
{
    _containedInScene = scene;
    for (auto &component : _components) {
        component->_containedInScene = scene;
    }
    for (auto &child : __childObjects) {
        child->propagateOwnedByScene(scene);
    }
}

void
jleObject::propagateOwnedBySceneClient(jleSceneClient *scene)
{
    _containedInScene = scene;
    _containedInSceneClient = scene;
    for (auto &component : _components) {
        component->_containedInScene = scene;
        component->_containedInSceneClient = scene;
    }
    for (auto &child : __childObjects) {
        child->propagateOwnedBySceneClient(scene);
    }
}

void
jleObject::propagateOwnedBySceneServer(jleSceneServer *scene)
{
    _containedInScene = scene;
    _containedInSceneServer = scene;
    for (auto &component : _components) {
        component->_containedInScene = scene;
        component->_containedInSceneServer = scene;
    }
    for (auto &child : __childObjects) {
        child->propagateOwnedBySceneServer(scene);
    }
}

void
jleObject::replaceChildrenWithTemplate()
{
    for (auto &&object : __childObjects) {
        // Replace child object with template object, if it is based on one
        if (object->__templatePath.has_value()) {
            auto path = object->__templatePath;
            try {
                auto original = gEngine->resources().loadResourceFromFile<jleObject>(object->__templatePath.value());

                auto copy = original->duplicateTemplate();
                object = copy;

                object->__templatePath = path;
            } catch (std::exception &e) {
                LOGE << "Failed to load object template: " << e.what();
            }
        }

        object->replaceChildrenWithTemplate();
    }
}

void
jleObject::propagateDestroy(jleEngineModulesContext& ctx)
{
    for (auto &&c : _components) {
        c->onDestroy(ctx);
        if (c->parallelUpdateEnabled() && !ctx.gameRuntime.isGameKilled()) {
            ctx.gameRuntime.getGame().removeParallelComponent(c);
        }
    }

    for (auto &&o : __childObjects) {
        o->propagateDestroy(ctx);
    }
}

void
jleObject::addComponentStart(const std::shared_ptr<jleComponent> &c, jleEngineModulesContext& ctx)
{
    if (!ctx.gameRuntime.isGameKilled()) {

        if (networkObjectType() == jleObjectNetworkType::SERVER) {
            c->serverStart(ctx);

            auto event = jleMakeNetEvent<jleAddComponentEvent>();
            event->component = c;
            event->objectNetId = netID();
            _containedInSceneServer->sendNetworkEventBroadcast(std::move(event));
        } else {
            c->_attachedToObject = this;
            c->start(ctx);
        }
        c->_isStarted = true;
    }
}

bool
jleObject::pendingKill()
{
    return _pendingKill;
}
jleObjectNetworkType
jleObject::networkObjectType()
{
    if (_containedInSceneClient) {
        return jleObjectNetworkType::CLIENT;
    }
    if (_containedInSceneServer) {
        return jleObjectNetworkType::SERVER;
    }
    return jleObjectNetworkType::REGULAR;
}
