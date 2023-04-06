// Copyright (c) 2023. Johan Lind

#include "jleObject.h"

#include "jlePathDefines.h"
#include "jleScene.h"
#include "jleTransform.h"

#include <filesystem>
#include <fstream>
#include <optional>

jleObject::jleObject() : _transform{this} {}

void
jleObject::destroyComponent(jleComponent *component)
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        if (_components[i].get() == component) {
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

/*
void
jleObject::saveObjectTemplate(jlePath &path)
{
    std::string sceneSavePath;
    if (!path.relativePathStr().empty()) {
        sceneSavePath = path.absolutePathStr();
    } else {
        sceneSavePath = GAME_RESOURCES_DIRECTORY + "/otemps/" + _instanceName + ".tmpl";
    }

    std::filesystem::create_directories(GAME_RESOURCES_DIRECTORY + "/otemps");
    std::ofstream sceneSave{sceneSavePath};

    //nlohmann::json j;
    //to_json(j, weakPtrToThis().lock());
    //sceneSave << j.dump(4);
    sceneSave.close();
}

void
jleObject::injectTemplate(const nlohmann::json &json)
{
    auto &&thiz = shared_from_this();
}

std::shared_ptr<jleObject>
jleObject::spawnChildObjectFromTemplate(const jlePath &path)
{
    std::ifstream i(path.absolutePathStr());
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        std::string objectsName;
        j.at("__obj_name").get_to(objectsName);
        std::cout << objectsName;

        auto spawnedObjFromJson = spawnChildObject(objectsName);
        spawnedObjFromJson->injectTemplate(j);
        spawnedObjFromJson->_templatePath = path.relativePathStr();

        return spawnedObjFromJson;
    }
    return nullptr;
}*/

void
jleObject::startComponents()
{
    for (int i = _components.size() - 1; i >= 0; i--) {
        _components[i]->start();
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
jleObject::updateChildren(float dt)
{
    for (int32_t i = __childObjects.size() - 1; i >= 0; i--) {
        if (__childObjects[i]->_pendingKill) {
            __childObjects.erase(__childObjects.begin() + i);
            continue;
        }

        __childObjects[i]->update(dt);
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

        __childObjects[i]->editorUpdate(dt);
        __childObjects[i]->updateComponentsEditor(dt);

        // Recursively update children after this object has updated
        __childObjects[i]->updateChildrenEditor(dt);
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
    duplicated->__instanceID = _containedInScene->getNextInstanceId();
    duplicated->_transform._owner = duplicated.get();

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

/*
nlohmann::json
jleObject::objectTemplateJson(const jlePath &path)
{

    // TODO: use caching
    std::ifstream i(path.absolutePathStr());
    if (i.good()) {
        nlohmann::json templateJson;
        i >> templateJson;
        return templateJson;
    } else {
        LOGE << "Failed loading JSON data with path " << path.absolutePathStr();
    }

    return {};
} */

int
jleObject::instanceID() const
{
    return __instanceID;
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
    for (auto child : __childObjects) {
        child->propagateOwnedByScene(scene);
    }
}
