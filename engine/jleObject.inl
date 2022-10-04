// Copyright (c) 2022. Johan Lind

#include "jleComponent.h"
#include "jleObject.h"
#include "jleScene.h"

template <typename T>
inline std::shared_ptr<T> jleObject::addComponent() {
    static_assert(std::is_base_of<jleComponent, T>::value,
                  "T must derive from jleComponent");

    for (auto& component : _components) {
        if (std::dynamic_pointer_cast<T>(component)) {
            return std::dynamic_pointer_cast<T>(component);
        }
    }

    std::shared_ptr<T> newComponent =
        std::make_shared<T>(this, _containedInScene);
    _components.push_back(newComponent);

    return newComponent;
};

template <typename T>
inline std::shared_ptr<T> jleObject::addCustomComponent(bool start_immediate) {
    static_assert(std::is_base_of<jleComponent, T>::value,
                  "T must derive from jleComponent");

    std::shared_ptr<T> newCustomComponent = addComponent<T>();
    _dynamicCustomComponents.push_back(newCustomComponent);

    if (start_immediate) {
        newCustomComponent->start();
    }

    return newCustomComponent;
};

inline std::shared_ptr<jleComponent> jleObject::addComponent(
    const std::string& component_name) {
    auto newComponent =
        jleTypeReflectionUtils::instantiateComponentByString(component_name);
    newComponent->_attachedToObject = this;
    newComponent->_containedInScene = _containedInScene;

    _components.push_back(newComponent);

    return newComponent;
}

inline std::shared_ptr<jleComponent> jleObject::addCustomComponent(
    const std::string& component_name, bool start_immediate) {
    auto newCustomComponent = addComponent(component_name);
    _dynamicCustomComponents.push_back(newCustomComponent);

    if (start_immediate) {
        newCustomComponent->start();
    }

    return newCustomComponent;
}

template <typename T>
inline std::shared_ptr<T> jleObject::component() {
    static_assert(std::is_base_of<jleComponent, T>::value,
                  "T must derive from jleComponent");

    for (auto& component : _components) {
        if (std::dynamic_pointer_cast<T>(component)) {
            return std::dynamic_pointer_cast<T>(component);
        }
    }

    return nullptr;
};

template <typename T>
inline std::shared_ptr<T> jleObject::addDependencyComponent(
    const jleComponent *component) {
    static_assert(std::is_base_of<jleComponent, T>::value,
                  "T must derive from jleComponent");

    std::shared_ptr<T> c = component->_attachedToObject->component<T>();
    if (!c) {
        c = component->_attachedToObject->addCustomComponent<T>();
    }

    return c;
}

template <typename T>
inline std::shared_ptr<T> jleObject::spawnChildObject() {
    // We still want the object to be spawned initially in the scene,
    // but to immediately be moved over to the object's ownership.
    // This is because the scene will run the start() methods on the new object.
    std::shared_ptr<T> object = _containedInScene->spawnObject<T>();
    attachChildObject(object);
    return object;
}

inline std::shared_ptr<jleObject> jleObject::spawnChildObject(
    const std::string& objName) {
    auto object = _containedInScene->spawnObject(objName);
    attachChildObject(object);
    return object;
}
