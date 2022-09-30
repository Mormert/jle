// Copyright (c) 2022. Johan Lind

#include "jleScene.h"
#include "jleObject.h"
#include "jleProfiler.h"

#include <iostream>

int jleScene::mScenesCreatedCount{0};

jleScene::jleScene() {
    mSceneName = "Scene_" + std::to_string(mScenesCreatedCount);
    mScenesCreatedCount++;
}

jleScene::jleScene(const std::string& sceneName) {
    this->mSceneName = sceneName;
    mScenesCreatedCount++;
}

void jleScene::UpdateSceneObjects(float dt) {
    JLE_SCOPE_PROFILE(jleScene::UpdateSceneObjects)
    for (int32_t i = mSceneObjects.size() - 1; i >= 0; i--) {
        if (mSceneObjects[i]->mPendingKill) {
            mSceneObjects.erase(mSceneObjects.begin() + i);
            continue;
        }

        mSceneObjects[i]->Update(dt);
        mSceneObjects[i]->UpdateComponents(dt);
        mSceneObjects[i]->UpdateChildren(dt);
    }
}

void jleScene::ProcessNewSceneObjects() {
    JLE_SCOPE_PROFILE(jleScene::ProcessNewSceneObjects)
    if (!mNewSceneObjects.empty()) {
        for (const auto& newObject : mNewSceneObjects) {
            if (!newObject->mIsStarted) {
                newObject->Start();
                newObject->StartComponents();
                newObject->mIsStarted = true;
            }

            // Only push back objects existing directly in the scene into scene
            // objects The object can be placed as a child object in another
            // object, and thus no longer existing directly in the scene
            if (newObject->mParentObject == nullptr) {
                mSceneObjects.push_back(newObject);
            }
        }

        mNewSceneObjects.clear();
    }
}

void jleScene::DestroyScene() {
    bPendingSceneDestruction = true;

    mSceneObjects.clear();
    mNewSceneObjects.clear();

    OnSceneDestruction();
}

std::shared_ptr<jleObject> jleScene::SpawnTemplateObject(
    const jleRelativePath& templatePath) {
    auto j = jleObject::GetObjectTemplateJson(templatePath);

    std::string objectsName;
    j.at("__obj_name").get_to(objectsName);

    auto spawnedObjFromJson = SpawnObject(objectsName);
    spawnedObjFromJson->mTemplatePath = templatePath.GetRelativePathStr();
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
    obj->mContainedInScene = this;
    obj->SetupDefaultObject();
    obj->mInstanceName = std::string{obj->GetObjectNameVirtual()} + "_" +
                         std::to_string(obj->sObjectsCreatedCount);

    mNewSceneObjects.push_back(obj);
}

void jleScene::ToJson(nlohmann::json& j_out) {
    j_out["_objects"] = mSceneObjects;
    j_out["_sceneName"] = mSceneName;
}

void jleScene::FromJson(const nlohmann::json& j_in) {
    JLE_FROM_JSON_IF_EXISTS(j_in, mSceneName, "_sceneName");

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
        spawnedObjFromJson->mTemplatePath = objectTemplatePath;
        from_json(object_json, spawnedObjFromJson);
        spawnedObjFromJson->FromJson(object_json);
    }
}