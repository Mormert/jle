// Copyright (c) 2022. Johan Lind

#include "jleObject.h"

#include "jlePathDefines.h"
#include "jleScene.h"
#include "cTransform.h"

#include <filesystem>
#include <fstream>
#include <optional>

jle::jleObject::jleObject() {
    mInstanceName = "jleObject_" + std::to_string(sObjectsCreatedCount);
    mInstanceID = sObjectsCreatedCount;
    sObjectsCreatedCount++;
}

void jle::jleObject::DestroyComponent(jleComponent *component) {
    for (int i = mComponents.size() - 1; i >= 0; i--) {
        if (mComponents[i].get() == component) {
            mComponents.erase(mComponents.begin() + i);
        }
    }
    for (int i = mDynamicCustomComponents.size() - 1; i >= 0; i--) {
        if (mDynamicCustomComponents[i].get() == component) {
            mDynamicCustomComponents.erase(mDynamicCustomComponents.begin() + i);
        }
    }
}

void jle::jleObject::DestroyObject() {
    mPendingKill = true;
}

int jle::jleObject::GetComponentCount() {
    return mComponents.size();
}

std::vector<std::shared_ptr<jle::jleComponent>> &jle::jleObject::GetCustomComponents() {
    return mDynamicCustomComponents;
}

std::vector<std::shared_ptr<jle::jleObject>> &jle::jleObject::GetChildObjects() {
    return mChildObjects;
}

void jle::jleObject::AttachChildObject(const std::shared_ptr<jleObject> &object) {
    // The objects must live in the same scene
    assert(object->mContainedInScene == mContainedInScene);
    if (object->mParentObject) {
        // Remove the object from previous parent, if any
        auto it = std::find(object->mParentObject->mChildObjects.begin(),
                            object->mParentObject->mChildObjects.end(), object);
        object->mParentObject->mChildObjects.erase(it);
    } else {
        // Else the object is directly in the scene
        // Remove the object from existing directly in the scene, and instead as a node under this object
        if (object->mIsStarted) {
            auto it = std::find(mContainedInScene->mSceneObjects.begin(), mContainedInScene->mSceneObjects.end(),
                                object);
            if (it != mContainedInScene->mSceneObjects.end()) {
                mContainedInScene->mSceneObjects.erase(it);
            }
        }
    }

    object->mParentObject = this;
    mChildObjects.push_back(object);

    if (auto t = object->GetComponent<cTransform>()) {
        t->SetDirty();
    }
}

void jle::jleObject::DetachObjectFromParent() {
    if (mParentObject) {
        int i = 0;
        std::shared_ptr<jleObject> thiz;
        for (auto &&o: mParentObject->mChildObjects) {
            if (o.get() == this) {
                thiz = o;
                mParentObject->mChildObjects.erase(mParentObject->mChildObjects.begin() + i);
                break;
            }
            i++;
        }
        // Insert this object to be contained directly in the scene
        mContainedInScene->mSceneObjects.push_back(thiz);
    }

    mParentObject = nullptr;
}

jle::jleObject::jleObject(jleScene *scene) : mContainedInScene{scene} {
}

void jle::jleObject::SaveObjectTemplate(jleRelativePath& path) {
    std::string sceneSavePath;
    if (!path.GetRelativePathStr().empty()) {
        sceneSavePath = path.GetAbsolutePathStr();
    } else {
        sceneSavePath = GAME_RESOURCES_DIRECTORY + "/otemps/" + mInstanceName + ".tmpl";
    }

    std::filesystem::create_directories(GAME_RESOURCES_DIRECTORY + "/otemps");
    std::ofstream sceneSave{sceneSavePath};

    nlohmann::json j;
    to_json(j, GetWeakPtrToThis().lock());
    sceneSave << j.dump(4);
    sceneSave.close();
}

void jle::jleObject::InjectTemplate(const nlohmann::json& json) {
    auto&& thiz = shared_from_this();
    jle::from_json(json, thiz);
    FromJson(json);
}

std::shared_ptr<jle::jleObject> jle::jleObject::SpawnChildObjectFromTemplate(const jleRelativePath& path) {
    std::ifstream i(path.GetAbsolutePathStr());
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        std::string objectsName;
        j.at("__obj_name").get_to(objectsName);
        std::cout << objectsName;

        auto spawnedObjFromJson = SpawnChildObject(objectsName);
        spawnedObjFromJson->InjectTemplate(j);
        spawnedObjFromJson->mTemplatePath = path.GetRelativePathStr();

        return spawnedObjFromJson;

    }
    return nullptr;
}


void jle::jleObject::StartComponents() {
    for (int i = mComponents.size() - 1; i >= 0; i--) {
        mComponents[i]->Start();
    }
}

void jle::jleObject::UpdateComponents(float dt) {
    for (int i = mComponents.size() - 1; i >= 0; i--) {
        mComponents[i]->Update(dt);
    }
}

void jle::jleObject::UpdateChildren(float dt) {
    for (int32_t i = mChildObjects.size() - 1; i >= 0; i--) {
        if (mChildObjects[i]->mPendingKill) {
            mChildObjects.erase(mChildObjects.begin() + i);
            continue;
        }

        mChildObjects[i]->Update(dt);
        mChildObjects[i]->UpdateComponents(dt);

        // Recursively update children after this object has updated
        mChildObjects[i]->UpdateChildren(dt);
    }
}

jle::jleObject *jle::jleObject::GetParent() {
    return mParentObject;
}

std::weak_ptr<jle::jleObject> jle::jleObject::GetWeakPtrToThis() {
    return weak_from_this();
}

void jle::to_json(nlohmann::json &j, const std::shared_ptr<jleObject> &o) {
    // If this object is based on a template object, then only save that reference
    if (o->mTemplatePath.has_value()) {
        j = nlohmann::json{
                {"_otemp",         o->mTemplatePath.value()},
                {"_instance_name", o->mInstanceName}
        };
        return;
    }

    j = nlohmann::json{
            {"__obj_name",         o->GetObjectNameVirtual()},
            {"_instance_name",     o->mInstanceName},
            {"_custom_components", o->mDynamicCustomComponents},
            {"_childObjects",      o->mChildObjects}
    };

    o->ToJson(j);
}

std::shared_ptr<jle::jleObject> jle::jleObject::ProcessChildJsonData(const nlohmann::json &j, std::shared_ptr<jleObject> &o) {
    std::string objectsName, instanceName;
    j.at("__obj_name").get_to(objectsName);
    j.at("_instance_name").get_to(instanceName);

    std::optional<std::shared_ptr<jleObject>> existingObject;
    for (auto &&existing_object: o->GetChildObjects()) {
        if (existing_object->mInstanceName == instanceName) {
            existingObject = existing_object;
            break;
        }
    }

    if (existingObject.has_value()) {
        existingObject->get()->FromJson(j);
        return existingObject.value();
    } else {
        // Recursively go through child objects
        auto spawnedChildObj = o->SpawnChildObject(objectsName);
        jle::from_json(j, spawnedChildObj);
        spawnedChildObj->FromJson(j);
        return spawnedChildObj;
    }
}

void jle::jleObject::DuplicateObject_Editor() {
    nlohmann::json j;
    to_json(j, GetWeakPtrToThis().lock());

    mContainedInScene->SpawnObject(j);
}

void jle::jleObject::ProcessJsonData(const nlohmann::json &j, std::shared_ptr<jleObject> &o) {
    JLE_FROM_JSON_IF_EXISTS(j, o->mInstanceName, "_instance_name")

    for (auto &&custom_components_json: j.at("_custom_components")) {
        std::string componentName;
        custom_components_json.at("_comp_name").get_to(componentName);

        std::optional<std::shared_ptr<jleComponent>> existingComponent;
        for (auto &&existing_component: o->GetCustomComponents()) {
            if (existing_component->GetComponentName() == componentName) {
                existingComponent = existing_component;
                break;
            }
        }

        if (existingComponent.has_value()) {
            existingComponent->get()->FromJson(custom_components_json);
        } else {
            auto component = o->AddCustomComponent(componentName);
            component->FromJson(custom_components_json);
        }
    }

    if (j.find("_childObjects") != j.end()) {
        for (auto object_json: j.at("_childObjects")) {
            if (object_json.find("_otemp") != object_json.end()) {
                const std::string objectTemplatePath = object_json.at("_otemp");
                const std::string objectInstanceName = object_json.at("_instance_name");
                auto templateJson = jleObject::GetObjectTemplateJson(jleRelativePath{objectTemplatePath});
                templateJson["_instance_name"] = objectInstanceName;
                auto newChildObject = ProcessChildJsonData(templateJson, o);
                newChildObject->mTemplatePath = objectTemplatePath;
            } else {
                ProcessChildJsonData(object_json, o);
            }
        }
    }
}

nlohmann::json jle::jleObject::GetObjectTemplateJson(const jleRelativePath &path) {

    // TODO: use caching
    std::ifstream i(path.GetAbsolutePathStr());
    if (i.good()) {
        nlohmann::json templateJson;
        i >> templateJson;
        return templateJson;
    } else {
        LOGE << "Failed loading JSON data with path " << path.GetAbsolutePathStr();
    }

    return {};
}

void jle::from_json(const nlohmann::json &json, std::shared_ptr<jleObject> &object) {

    // Check if this object is based on an object template
    const auto otemp_it = json.find("_otemp");
    if (otemp_it != json.end()) {
        const std::string objectTemplatePath = json.at("_otemp");
        const std::string objectInstanceName = json.at("_instance_name");
        const auto templateJson = jleObject::GetObjectTemplateJson(jleRelativePath{objectTemplatePath});
        object->mTemplatePath = objectTemplatePath;

        jleObject::ProcessJsonData(templateJson, object);
        object->mInstanceName = objectInstanceName;

    } else {
        jleObject::ProcessJsonData(json, object);
    }
}

int jle::jleObject::GetInstanceID() const {
    return mInstanceID;
}

