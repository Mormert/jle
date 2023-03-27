// Copyright (c) 2023. Johan Lind

#include "jleScene.h"
#include "jleObject.h"
#include "jleProfiler.h"

#include <iostream>

int jleScene::_scenesCreatedCount{0};

jleScene::jleScene() {
    sceneName = "Scene_" + std::to_string(_scenesCreatedCount);
    _scenesCreatedCount++;
}

jleScene::jleScene(const std::string &sceneName) {
    this->sceneName = sceneName;
    _scenesCreatedCount++;
}

void jleScene::updateSceneObjects(float dt) {
    JLE_SCOPE_PROFILE_CPU(jleScene_updateSceneObjects)
    for (int32_t i = _sceneObjects.size() - 1; i >= 0; i--) {
        if (_sceneObjects[i]->_pendingKill) {
            _sceneObjects.erase(_sceneObjects.begin() + i);
            continue;
        }

        _sceneObjects[i]->update(dt);
        _sceneObjects[i]->updateComponents(dt);
        _sceneObjects[i]->updateChildren(dt);
    }
}

void jleScene::processNewSceneObjects() {
    JLE_SCOPE_PROFILE_CPU(jleScene_processNewSceneObjects)
    if (!_newSceneObjects.empty()) {
        for (const auto &newObject : _newSceneObjects) {
            if (!newObject->_isStarted) {
                newObject->start();
                newObject->startComponents();
                newObject->_isStarted = true;
            }

            // Only push back objects existing directly in the scene into scene
            // objects The object can be placed as a child object in another
            // object, and thus no longer existing directly in the scene
            if (newObject->_parentObject == nullptr) {
                _sceneObjects.push_back(newObject);
            }
        }

        _newSceneObjects.clear();
    }
}

void jleScene::destroyScene() {
    bPendingSceneDestruction = true;

    _sceneObjects.clear();
    _newSceneObjects.clear();

    onSceneDestruction();
}

std::shared_ptr<jleObject> jleScene::spawnTemplateObject(
    const jleRelativePath &templatePath) {
    auto j = jleObject::objectTemplateJson(templatePath);

    std::string objectsName;
    j.at("__obj_name").get_to(objectsName);

    auto spawnedObjFromJson = spawnObject(objectsName);
    spawnedObjFromJson->_templatePath = templatePath.relativePathStr();
    from_json(j, spawnedObjFromJson);
    spawnedObjFromJson->fromJson(j);

    return spawnedObjFromJson;
}

std::shared_ptr<jleObject> jleScene::spawnObject(const nlohmann::json &j_in) {
    std::string objectsName;
    j_in.at("__obj_name").get_to(objectsName);

    auto spawnedObjFromJson = spawnObject(objectsName);
    from_json(j_in, spawnedObjFromJson);
    spawnedObjFromJson->fromJson(j_in);

    return spawnedObjFromJson;
}

void to_json(nlohmann::json &j, jleScene &s) { s.toJson(j); }

void from_json(const nlohmann::json &j, jleScene &s) { s.fromJson(j); }

void jleScene::configurateSpawnedObject(const std::shared_ptr<jleObject> &obj) {
    obj->_containedInScene = this;
    obj->setupDefaultObject();
    obj->__instanceID = getNextInstanceId();
    obj->_instanceName = std::string{obj->objectNameVirtual()} + "_" +
                         std::to_string(obj->__instanceID);

    _newSceneObjects.push_back(obj);
}

void jleScene::toJson(nlohmann::json &j_out) {
    j_out["_objects"] = _sceneObjects;
    j_out["_sceneName"] = sceneName;
}

void jleScene::fromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_IF_EXISTS(j_in, sceneName, "_sceneName");

    for (auto object_json : j_in.at("_objects")) {

        std::optional<std::string> objectTemplatePath{};
        // Is this object based on a template?
        if (object_json.find("_otemp") != object_json.end()) {
            const std::string path = object_json.at("_otemp");
            const std::string objectInstanceName =
                object_json.at("_instance_name");
            const auto templateJson =
                jleObject::objectTemplateJson(jleRelativePath{path});
            object_json =
                templateJson; // replace with template object json instead
            object_json["_instance_name"] =
                objectInstanceName; // overwrite instance name
            objectTemplatePath = path;
        }

        std::string objectsName;
        object_json.at("__obj_name").get_to(objectsName);

        auto spawnedObjFromJson = spawnObject(objectsName);
        spawnedObjFromJson->_templatePath = objectTemplatePath;
        from_json(object_json, spawnedObjFromJson);
        spawnedObjFromJson->fromJson(object_json);
    }
}
void
jleScene::startObjects()
{
    for(auto&& o : _sceneObjects)
    {
        startObject(&*o);
    }
}

void
jleScene::startObject(jleObject *o)
{
    if (!o->_isStarted) {
        o->start();
        o->startComponents();
        o->_isStarted = true;
        for(auto&& c : o->__childObjects)
        {
            startObject(&*c);
        }
    }
}

uint32_t
jleScene::getNextInstanceId()
{
    return _objectsInstantiatedCounter++;
}
