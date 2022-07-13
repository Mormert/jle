// Copyright (c) 2022. Johan Lind

#include "jleScene.h"
#include "jleObject.h"

#include <iostream>

int jle::jleScene::mScenesCreatedCount{0};

jle::jleScene::jleScene() {
    mSceneName = "Scene_" + std::to_string(mScenesCreatedCount);
    mScenesCreatedCount++;
}

jle::jleScene::jleScene(const std::string &sceneName) {
    this->mSceneName = sceneName;
    mScenesCreatedCount++;
}

void jle::jleScene::UpdateSceneObjects(float dt) {
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

void jle::jleScene::ProcessNewSceneObjects() {
    if (!mNewSceneObjects.empty()) {
        for (const auto &newObject: mNewSceneObjects) {
            if (!newObject->mIsStarted) {
                newObject->Start();
                newObject->StartComponents();
                newObject->mIsStarted = true;
            }

            // Only push back objects existing directly in the scene into scene objects
            // The object can be placed as a child object in another object, and thus
            // no longer existing directly in the scene
            if (newObject->mParentObject == nullptr) {
                mSceneObjects.push_back(newObject);
            }
        }

        mNewSceneObjects.clear();
    }
}

void jle::jleScene::DestroyScene() {
    bPendingSceneDestruction = true;

    mSceneObjects.clear();
    mNewSceneObjects.clear();

    OnSceneDestruction();
}

std::shared_ptr<jle::jleObject> jle::jleScene::SpawnTemplateObject(const std::string &templatePath) {
    auto j = jleObject::GetObjectTemplateJson(templatePath);

    std::string objectsName;
    j.at("__obj_name").get_to(objectsName);

    auto spawnedObjFromJson = SpawnObject(objectsName);
    spawnedObjFromJson->mTemplatePath = templatePath;
    jle::from_json(j, spawnedObjFromJson);
    spawnedObjFromJson->FromJson(j);

    return spawnedObjFromJson;
}

void jle::to_json(nlohmann::json &j, const jleScene &s) {
    j = nlohmann::json{
            {"objects",   s.mSceneObjects},
            {"sceneName", s.mSceneName}
    };
}

void jle::from_json(const nlohmann::json &j, jleScene &s) {
    JLE_FROM_JSON_IF_EXISTS(j, s.mSceneName, "sceneName");
    JLE_FROM_JSON_IF_EXISTS(j, s.mSceneName, "sceneName");

    for (auto object_json: j.at("objects")) {

        std::optional<std::string> objectTemplatePath{};
        // Is this object based on a template?
        if (object_json.find("_otemp") != object_json.end()) {
            const std::string path = object_json.at("_otemp");
            const std::string objectInstanceName = object_json.at("_instance_name");
            const auto templateJson = jleObject::GetObjectTemplateJson(path);
            object_json = templateJson; // replace with template object json instead
            object_json["_instance_name"] = objectInstanceName; // overwrite instance name
            objectTemplatePath = path;
        }

        std::string objectsName;
        object_json.at("__obj_name").get_to(objectsName);

        auto spawnedObjFromJson = s.SpawnObject(objectsName);
        spawnedObjFromJson->mTemplatePath = objectTemplatePath;
        jle::from_json(object_json, spawnedObjFromJson);
        spawnedObjFromJson->FromJson(object_json);

    }
}

void jle::jleScene::ConfigurateSpawnedObject(const std::shared_ptr<jleObject>& obj) {
    obj->mContainedInScene = this;
    obj->SetupDefaultObject();
    obj->mInstanceName = std::string{obj->GetObjectNameVirtual()} + "_" + std::to_string(obj->sObjectsCreatedCount);

    mNewSceneObjects.push_back(obj);
}