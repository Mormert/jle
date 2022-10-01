// Copyright (c) 2022. Johan Lind

#include "jleScene.h"
#include "jleObject.h"
#include "jleProfiler.h"

#include <iostream>

int jleScene::_scenesCreatedCount{0};

jleScene::jleScene() {
    _sceneName = "Scene_" + std::to_string(_scenesCreatedCount);
    _scenesCreatedCount++;
}

jleScene::jleScene(const std::string& sceneName) {
    this->_sceneName = sceneName;
    _scenesCreatedCount++;
}

void jleScene::UpdateSceneObjects(float dt) {
    JLE_SCOPE_PROFILE(jleScene::UpdateSceneObjects)
    for (int32_t i = _sceneObjects.size() - 1; i >= 0; i--) {
        if (_sceneObjects[i]->_pendingKill) {
            _sceneObjects.erase(_sceneObjects.begin() + i);
            continue;
        }

        _sceneObjects[i]->Update(dt);
        _sceneObjects[i]->UpdateComponents(dt);
        _sceneObjects[i]->UpdateChildren(dt);
    }
}

void jleScene::ProcessNewSceneObjects() {
    JLE_SCOPE_PROFILE(jleScene::ProcessNewSceneObjects)
    if (!_newSceneObjects.empty()) {
        for (const auto& newObject : _newSceneObjects) {
            if (!newObject->_isStarted) {
                newObject->Start();
                newObject->StartComponents();
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

void jleScene::DestroyScene() {
    bPendingSceneDestruction = true;

    _sceneObjects.clear();
    _newSceneObjects.clear();

    OnSceneDestruction();
}

std::shared_ptr<jleObject> jleScene::SpawnTemplateObject(
    const jleRelativePath& templatePath) {
    auto j = jleObject::GetObjectTemplateJson(templatePath);

    std::string objectsName;
    j.at("__obj_name").get_to(objectsName);

    auto spawnedObjFromJson = SpawnObject(objectsName);
    spawnedObjFromJson->_templatePath = templatePath.GetRelativePathStr();
    from_json(j, spawnedObjFromJson);
    spawnedObjFromJson->FromJson(j);

    return spawnedObjFromJson;
}

std::shared_ptr<jleObject> jleScene::SpawnObject(const nlohmann::json& j_in) {
    std::string objectsName;
    j_in.at("__obj_name").get_to(objectsName);

    auto spawnedObjFromJson = SpawnObject(objectsName);
    from_json(j_in, spawnedObjFromJson);
    spawnedObjFromJson->FromJson(j_in);

    return spawnedObjFromJson;
}

void to_json(nlohmann::json& j, jleScene& s) { s.ToJson(j); }

void from_json(const nlohmann::json& j, jleScene& s) { s.FromJson(j); }

void jleScene::ConfigurateSpawnedObject(const std::shared_ptr<jleObject>& obj) {
    obj->_containedInScene = this;
    obj->SetupDefaultObject();
    obj->_instanceName = std::string{obj->GetObjectNameVirtual()} + "_" +
                         std::to_string(obj->sObjectsCreatedCount);

    _newSceneObjects.push_back(obj);
}

void jleScene::ToJson(nlohmann::json& j_out) {
    j_out["_objects"] = _sceneObjects;
    j_out["_sceneName"] = _sceneName;
}

void jleScene::FromJson(const nlohmann::json& j_in) {
    JLE_FROM_JSON_IF_EXISTS(j_in, _sceneName, "_sceneName");

    for (auto object_json : j_in.at("_objects")) {

        std::optional<std::string> objectTemplatePath{};
        // Is this object based on a template?
        if (object_json.find("_otemp") != object_json.end()) {
            const std::string path = object_json.at("_otemp");
            const std::string objectInstanceName =
                object_json.at("_instance_name");
            const auto templateJson =
                jleObject::GetObjectTemplateJson(jleRelativePath{path});
            object_json =
                templateJson; // replace with template object json instead
            object_json["_instance_name"] =
                objectInstanceName; // overwrite instance name
            objectTemplatePath = path;
        }

        std::string objectsName;
        object_json.at("__obj_name").get_to(objectsName);

        auto spawnedObjFromJson = SpawnObject(objectsName);
        spawnedObjFromJson->_templatePath = objectTemplatePath;
        from_json(object_json, spawnedObjFromJson);
        spawnedObjFromJson->FromJson(object_json);
    }
}