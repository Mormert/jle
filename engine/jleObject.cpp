// Copyright (c) 2022. Johan Lind

#include "jleObject.h"

#include "jleScene.h"

#include <optional>

int jle::jleObject::mObjectsCreatedCount{0};

jle::jleObject::jleObject() {
    mInstanceName = "jleObject_" + std::to_string(mObjectsCreatedCount);
    mObjectsCreatedCount++;
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


void jle::to_json(nlohmann::json &j, const std::shared_ptr<jleObject> &o) {
    j = nlohmann::json{
            {"__obj_name",         o->GetObjectNameVirtual()},
            {"_instance_name",     o->mInstanceName},
            {"_custom_components", o->mDynamicCustomComponents},
            {"_childObjects",      o->mChildObjects}
    };

    o->ToJson(j);
}

void jle::from_json(const nlohmann::json &j, std::shared_ptr<jleObject> &o) {

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
            std::string objectsName, instanceName;
            object_json.at("__obj_name").get_to(objectsName);
            object_json.at("_instance_name").get_to(instanceName);

            std::optional<std::shared_ptr<jleObject>> existingObject;
            for (auto &&existing_object: o->GetChildObjects()) {
                if (existing_object->mInstanceName == instanceName) {
                    existingObject = existing_object;
                    break;
                }
            }

            if (existingObject.has_value()) {
                existingObject->get()->FromJson(object_json);
            } else {
                auto spawnedChildObj = o->SpawnChildObject(objectsName);
                jle::from_json(object_json, spawnedChildObj);
                spawnedChildObj->FromJson(object_json);
            }
        }
    }
}
