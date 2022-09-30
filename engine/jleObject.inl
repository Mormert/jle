// Copyright (c) 2022. Johan Lind

#include "jleComponent.h"
#include "jleObject.h"
#include "jleScene.h"

template <typename T>
inline std::shared_ptr<T> jleObject::AddComponent() {
    static_assert(std::is_base_of<jleComponent, T>::value,
                  "T must derive from jleComponent");

    for (auto& component : mComponents) {
        if (std::dynamic_pointer_cast<T>(component)) {
            return std::dynamic_pointer_cast<T>(component);
        }
    }

    std::shared_ptr<T> newComponent =
        std::make_shared<T>(this, mContainedInScene);
    mComponents.push_back(newComponent);

    return newComponent;
};

template <typename T>
inline std::shared_ptr<T> jleObject::AddCustomComponent(bool start_immediate) {
    static_assert(std::is_base_of<jleComponent, T>::value,
                  "T must derive from jleComponent");

    std::shared_ptr<T> newCustomComponent = AddComponent<T>();
    mDynamicCustomComponents.push_back(newCustomComponent);

    if (start_immediate) {
        newCustomComponent->Start();
    }

    return newCustomComponent;
};

inline std::shared_ptr<jleComponent> jleObject::AddComponent(
    const std::string& component_name) {
    auto newComponent =
        jleTypeReflectionUtils::InstantiateComponentByString(component_name);
    newComponent->mAttachedToObject = this;
    newComponent->mContainedInScene = mContainedInScene;

    mComponents.push_back(newComponent);

    return newComponent;
}

inline std::shared_ptr<jleComponent> jleObject::AddCustomComponent(
    const std::string& component_name, bool start_immediate) {
    auto newCustomComponent = AddComponent(component_name);
    mDynamicCustomComponents.push_back(newCustomComponent);

    if (start_immediate) {
        newCustomComponent->Start();
    }

    return newCustomComponent;
}

template <typename T>
inline std::shared_ptr<T> jleObject::GetComponent() {
    static_assert(std::is_base_of<jleComponent, T>::value,
                  "T must derive from jleComponent");

    for (auto& component : mComponents) {
        if (std::dynamic_pointer_cast<T>(component)) {
            return std::dynamic_pointer_cast<T>(component);
        }
    }

    return nullptr;
};

template <typename T>
inline std::shared_ptr<T> jleObject::AddDependencyComponent(
    const jleComponent *component) {
    static_assert(std::is_base_of<jleComponent, T>::value,
                  "T must derive from jleComponent");

    std::shared_ptr<T> c = component->mAttachedToObject->GetComponent<T>();
    if (!c) {
        c = component->mAttachedToObject->AddCustomComponent<T>();
    }

    return c;
}

template <typename T>
inline std::shared_ptr<T> jleObject::SpawnChildObject() {
    // We still want the object to be spawned initially in the scene,
    // but to immediately be moved over to the object's ownership.
    // This is because the scene will run the Start() methods on the new object.
    std::shared_ptr<T> object = mContainedInScene->SpawnObject<T>();
    AttachChildObject(object);
    return object;
}

inline std::shared_ptr<jleObject> jleObject::SpawnChildObject(
    const std::string& objName) {
    auto object = mContainedInScene->SpawnObject(objName);
    AttachChildObject(object);
    return object;
}
