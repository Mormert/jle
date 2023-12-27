// Copyright (c) 2023. Johan Lind

#include "jleObject.h"
#include "cLuaScript.h"
#include "jleGame.h"
#include "jleGameEngine.h"
#include "jlePathDefines.h"
#include "jleScene.h"
#include "jleTransform.h"

#include <filesystem>
#include <fstream>
#include <optional>

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleObject)

jleObject::jleObject() : _transform{this} { _instanceID = _instanceIdCounter++; }

void
jleObject::destroyComponent(jleComponent *component)
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        if (_components[i].get() == component) {
            if (!gEngine->isGameKilled()) {

                if (auto luaScriptComponent = getComponent<cLuaScript>()) {
                    luaScriptComponent->getSelf()[component->componentName()] = sol::lua_nil;
                }

                component->onDestroy();
            }
            component->_isDestroyed = true;
            _components.erase(_components.begin() + i);
        }
    }
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
jleObject::customComponents()
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
    assert(object->_containedInScene == _containedInScene);
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

jleObject::jleObject(jleScene *scene) : _containedInScene{scene}, _transform{this} {}

void
jleObject::startComponents()
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        _components[i]->start();
        if (_components[i]->_enableParallelUpdate) {
            gEngine->gameRef().addParallelComponent(_components[i]);
        }
    }
}

void
jleObject::updateComponents(float dt)
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        _components[i]->update(dt);
    }
}

void
jleObject::updateComponentsEditor(float dt)
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        _components[i]->editorUpdate(dt);
    }
}

void
jleObject::updateComponentsServer(float dt)
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        _components[i]->serverUpdate(dt);
    }
}

void
jleObject::updateChildren(float dt)
{
    for (int32_t i = __childObjects.size() - 1; i >= 0; i--) {
        if (__childObjects[i]->_pendingKill) {
            __childObjects[i]->propagateDestroy();
            __childObjects.erase(__childObjects.begin() + i);
            continue;
        }

        __childObjects[i]->updateComponents(dt);

        // Recursively update children after this object has updated
        __childObjects[i]->updateChildren(dt);
    }
}

void
jleObject::updateChildrenEditor(float dt)
{
    for (int32_t i = __childObjects.size() - 1; i >= 0; i--) {
        if (__childObjects[i]->_pendingKill) {
            __childObjects.erase(__childObjects.begin() + i);
            continue;
        }

        __childObjects[i]->updateComponentsEditor(dt);

        // Recursively update children after this object has updated
        __childObjects[i]->updateChildrenEditor(dt);
    }
}

void
jleObject::updateChildrenServer(float dt)
{
    for (int32_t i = __childObjects.size() - 1; i >= 0; i--) {
        if (__childObjects[i]->_pendingKill) {
            __childObjects.erase(__childObjects.begin() + i);
            continue;
        }

        __childObjects[i]->updateComponentsServer(dt);

        // Recursively update children after this object has updated
        __childObjects[i]->updateChildrenServer(dt);
    }
}

jleObject *
jleObject::parent()
{
    return _parentObject;
}

std::weak_ptr<jleObject>
jleObject::weakPtrToThis()
{
    return weak_from_this();
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
jleObject::saveAsObjectTemplate()
{
    std::ofstream save{jlePath{"GR:otemps/" + _instanceName}.getRealPath() + ".jobj"};
    cereal::JSONOutputArchive outputArchive(save);
    outputArchive(shared_from_this());
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
        outObject = shared_from_this();
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
jleObject::propagateDestroy()
{
    for (auto &&c : _components) {
        c->onDestroy();
        if (c->parallelUpdateEnabled() && !gEngine->isGameKilled()) {
            gEngine->gameRef().removeParallelComponent(c);
        }
    }

    for (auto &&o : __childObjects) {
        o->propagateDestroy();
    }
}

void
jleObject::addComponentStart(jleComponent *c)
{
    if (!gEngine->isGameKilled()) {

        if (auto luaComponent = getComponent<cLuaScript>()) {
            c->registerSelfLua(luaComponent->getSelf());
        }

        c->start();
    }
}

bool
jleObject::pendingKill()
{
    return _pendingKill;
}
