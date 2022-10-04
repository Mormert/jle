// Copyright (c) 2022. Johan Lind

#include "jleObject.h"

#include "cTransform.h"
#include "jlePathDefines.h"
#include "jleScene.h"

#include <filesystem>
#include <fstream>
#include <optional>

jleObject::jleObject() {
    _instanceName = "jleObject_" + std::to_string(sObjectsCreatedCount);
    _instanceID = sObjectsCreatedCount;
    sObjectsCreatedCount++;
}

void jleObject::destroyComponent(jleComponent *component) {
    for (int i = _components.size() - 1; i >= 0; i--) {
        if (_components[i].get() == component) {
            _components.erase(_components.begin() + i);
        }
    }
    for (int i = _dynamicCustomComponents.size() - 1; i >= 0; i--) {
        if (_dynamicCustomComponents[i].get() == component) {
            _dynamicCustomComponents.erase(_dynamicCustomComponents.begin() +
                                           i);
        }
    }
}

void jleObject::destroyObject() { _pendingKill = true; }

int jleObject::componentCount() { return _components.size(); }

std::vector<std::shared_ptr<jleComponent>>& jleObject::customComponents() {
    return _dynamicCustomComponents;
}

std::vector<std::shared_ptr<jleObject>>& jleObject::childObjects() {
    return _childObjects;
}

void jleObject::attachChildObject(const std::shared_ptr<jleObject>& object) {
    // The objects must live in the same scene
    assert(object->_containedInScene == _containedInScene);
    if (object->_parentObject) {
        // Remove the object from previous parent, if any
        auto it = std::find(object->_parentObject->_childObjects.begin(),
                            object->_parentObject->_childObjects.end(),
                            object);
        object->_parentObject->_childObjects.erase(it);
    }
    else {
        // Else the object is directly in the scene
        // Remove the object from existing directly in the scene, and instead as
        // a node under this object
        if (object->_isStarted) {
            auto it = std::find(_containedInScene->_sceneObjects.begin(),
                                _containedInScene->_sceneObjects.end(),
                                object);
            if (it != _containedInScene->_sceneObjects.end()) {
                _containedInScene->_sceneObjects.erase(it);
            }
        }
    }

    object->_parentObject = this;
    _childObjects.push_back(object);

    if (auto t = object->component<cTransform>()) {
        t->dirty();
    }
}

void jleObject::detachObjectFromParent() {
    if (_parentObject) {
        int i = 0;
        std::shared_ptr<jleObject> thiz;
        for (auto&& o : _parentObject->_childObjects) {
            if (o.get() == this) {
                thiz = o;
                _parentObject->_childObjects.erase(
                    _parentObject->_childObjects.begin() + i);
                break;
            }
            i++;
        }
        // Insert this object to be contained directly in the scene
        _containedInScene->_sceneObjects.push_back(thiz);
    }

    _parentObject = nullptr;
}

jleObject::jleObject(jleScene *scene) : _containedInScene{scene} {}

void jleObject::saveObjectTemplate(jleRelativePath& path) {
    std::string sceneSavePath;
    if (!path.relativePathStr().empty()) {
        sceneSavePath = path.absolutePathStr();
    }
    else {
        sceneSavePath =
            GAME_RESOURCES_DIRECTORY + "/otemps/" + _instanceName + ".tmpl";
    }

    std::filesystem::create_directories(GAME_RESOURCES_DIRECTORY + "/otemps");
    std::ofstream sceneSave{sceneSavePath};

    nlohmann::json j;
    to_json(j, weakPtrToThis().lock());
    sceneSave << j.dump(4);
    sceneSave.close();
}

void jleObject::injectTemplate(const nlohmann::json& json) {
    auto&& thiz = shared_from_this();
    from_json(json, thiz);
    fromJson(json);
}

std::shared_ptr<jleObject> jleObject::spawnChildObjectFromTemplate(
    const jleRelativePath& path) {
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
}

void jleObject::startComponents() {
    for (int i = _components.size() - 1; i >= 0; i--) {
        _components[i]->start();
    }
}

void jleObject::updateComponents(float dt) {
    for (int i = _components.size() - 1; i >= 0; i--) {
        _components[i]->update(dt);
    }
}

void jleObject::updateChildren(float dt) {
    for (int32_t i = _childObjects.size() - 1; i >= 0; i--) {
        if (_childObjects[i]->_pendingKill) {
            _childObjects.erase(_childObjects.begin() + i);
            continue;
        }

        _childObjects[i]->update(dt);
        _childObjects[i]->updateComponents(dt);

        // Recursively update children after this object has updated
        _childObjects[i]->updateChildren(dt);
    }
}

jleObject *jleObject::parent() { return _parentObject; }

std::weak_ptr<jleObject> jleObject::weakPtrToThis() { return weak_from_this(); }

void to_json(nlohmann::json& j, const std::shared_ptr<jleObject>& o) {
    // If this object is based on a template object, then only save that
    // reference
    if (o->_templatePath.has_value()) {
        j = nlohmann::json{{"_otemp", o->_templatePath.value()},
                           {"_instance_name", o->_instanceName}};
        return;
    }

    j = nlohmann::json{{"__obj_name", o->objectNameVirtual()},
                       {"_instance_name", o->_instanceName},
                       {"_custom_components", o->_dynamicCustomComponents},
                       {"_childObjects", o->_childObjects}};

    o->toJson(j);
}

std::shared_ptr<jleObject> jleObject::processChildJsonData(
    const nlohmann::json& j, std::shared_ptr<jleObject>& o) {
    std::string objectsName, instanceName;
    j.at("__obj_name").get_to(objectsName);
    j.at("_instance_name").get_to(instanceName);

    std::optional<std::shared_ptr<jleObject>> existingObject;
    for (auto&& existing_object : o->childObjects()) {
        if (existing_object->_instanceName == instanceName) {
            existingObject = existing_object;
            break;
        }
    }

    if (existingObject.has_value()) {
        existingObject->get()->fromJson(j);
        return existingObject.value();
    }
    else {
        // Recursively go through child objects
        auto spawnedChildObj = o->spawnChildObject(objectsName);
        from_json(j, spawnedChildObj);
        spawnedChildObj->fromJson(j);
        return spawnedChildObj;
    }
}

void jleObject::DuplicateObject_Editor() {
    nlohmann::json j;
    to_json(j, weakPtrToThis().lock());

    _containedInScene->spawnObject(j);
}

void jleObject::processJsonData(const nlohmann::json& j,
                                std::shared_ptr<jleObject>& o) {
    JLE_FROM_JSON_IF_EXISTS(j, o->_instanceName, "_instance_name")

    for (auto&& custom_components_json : j.at("_custom_components")) {
        std::string componentName;
        custom_components_json.at("_comp_name").get_to(componentName);

        std::optional<std::shared_ptr<jleComponent>> existingComponent;
        for (auto&& existing_component : o->customComponents()) {
            if (existing_component->componentName() == componentName) {
                existingComponent = existing_component;
                break;
            }
        }

        if (existingComponent.has_value()) {
            existingComponent->get()->fromJson(custom_components_json);
        }
        else {
            auto component = o->addCustomComponent(componentName);
            component->fromJson(custom_components_json);
        }
    }

    if (j.find("_childObjects") != j.end()) {
        for (auto object_json : j.at("_childObjects")) {
            if (object_json.find("_otemp") != object_json.end()) {
                const std::string objectTemplatePath = object_json.at("_otemp");
                const std::string objectInstanceName =
                    object_json.at("_instance_name");
                auto templateJson = jleObject::objectTemplateJson(
                    jleRelativePath{objectTemplatePath});
                templateJson["_instance_name"] = objectInstanceName;
                auto newChildObject = processChildJsonData(templateJson, o);
                newChildObject->_templatePath = objectTemplatePath;
            }
            else {
                processChildJsonData(object_json, o);
            }
        }
    }
}

nlohmann::json jleObject::objectTemplateJson(const jleRelativePath& path) {

    // TODO: use caching
    std::ifstream i(path.absolutePathStr());
    if (i.good()) {
        nlohmann::json templateJson;
        i >> templateJson;
        return templateJson;
    }
    else {
        LOGE << "Failed loading JSON data with path " << path.absolutePathStr();
    }

    return {};
}

void from_json(const nlohmann::json& json, std::shared_ptr<jleObject>& object) {

    // Check if this object is based on an object template
    const auto otemp_it = json.find("_otemp");
    if (otemp_it != json.end()) {
        const std::string objectTemplatePath = json.at("_otemp");
        const std::string objectInstanceName = json.at("_instance_name");
        const auto templateJson =
            jleObject::objectTemplateJson(jleRelativePath{objectTemplatePath});
        object->_templatePath = objectTemplatePath;

        jleObject::processJsonData(templateJson, object);
        object->_instanceName = objectInstanceName;
    }
    else {
        jleObject::processJsonData(json, object);
    }
}

int jleObject::instanceID() const { return _instanceID; }
