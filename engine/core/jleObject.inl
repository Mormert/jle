/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "core/jleComponent.h"
#include "jleObject.h"
#include "jleScene.h"

#include <cereal/types/optional.hpp>

template <class Archive>
void
jleObject::serialize(Archive &archive)
{
    try {
        archive(CEREAL_NVP(__templatePath));
    } catch (std::exception &e) {
    }

    archive(CEREAL_NVP(_instanceName), CEREAL_NVP(_transform), CEREAL_NVP(__childObjects), CEREAL_NVP(_components));

    for (auto &&child : __childObjects) {
        child->_parentObject = this;
    }

    getTransform().propagateMatrixFromObjectSerialization();

    for (auto &&component : _components) {
        component->_attachedToObject = this;
        component->_containedInScene = _containedInScene;
    }
}
template <typename T>
inline std::shared_ptr<T>
jleObject::addComponent(jleEngineModulesContext &ctx)
{
    static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

    for (auto &component : _components) {
        if (std::dynamic_pointer_cast<T>(component)) {
            return std::dynamic_pointer_cast<T>(component);
        }
    }

    std::shared_ptr<T> newComponent = std::make_shared<T>();
    newComponent->_attachedToObject = this;
    newComponent->_containedInScene = _containedInScene;

    _components.push_back(newComponent);

    addComponentStart(newComponent, ctx);

    return newComponent;
};

inline std::shared_ptr<jleComponent>
jleObject::addComponentByName(const std::string &component_name, jleEngineModulesContext &ctx)
{
    auto newComponent = jleTypeReflectionUtils::instantiateComponentByString(component_name);
    if (!newComponent) {
        LOGE << "Attempting to add non-existent component: " << component_name;
        return nullptr;
    }
    newComponent->_attachedToObject = this;
    newComponent->_containedInScene = _containedInScene;

    if (networkObjectType() == jleObjectNetworkType::SERVER) {
        newComponent->_containedInSceneServer = _containedInSceneServer;
    } else if (networkObjectType() == jleObjectNetworkType::CLIENT) {
        newComponent->_containedInSceneClient = _containedInSceneClient;
    }

    _components.push_back(newComponent);

    addComponentStart(newComponent, ctx);

    return newComponent;
}

template <typename T>
void
jleObject::addComponent(const std::shared_ptr<T> &component, jleEngineModulesContext &ctx)
{
    static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

    const auto &c = std::static_pointer_cast<jleComponent>(component);
    c->_attachedToObject = this;
    c->_containedInScene = _containedInScene;

    if (networkObjectType() == jleObjectNetworkType::SERVER) {
        c->_containedInSceneServer = _containedInSceneServer;
    } else if (networkObjectType() == jleObjectNetworkType::CLIENT) {
        c->_containedInSceneClient = _containedInSceneClient;
    }

    _components.push_back(component);

    addComponentStart(component, ctx);
}

template <typename T>
inline std::shared_ptr<T>
jleObject::getComponent()
{
    static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

    for (auto &component : _components) {
        if (std::dynamic_pointer_cast<T>(component)) {
            return std::dynamic_pointer_cast<T>(component);
        }
    }

    return nullptr;
};

template <typename T>
std::shared_ptr<T>
jleObject::getComponentInChildren(jleObject *object)
{
    static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

    if (!object) {
        object = this;
    }

    for (auto &component : object->_components) {
        if (std::dynamic_pointer_cast<T>(component)) {
            return std::dynamic_pointer_cast<T>(component);
        }
    }

    for (auto &child : object->__childObjects) {
        return getComponentInChildren<T>(child.get());
    }

    return nullptr;
}

template <typename T>
inline std::shared_ptr<T>
jleObject::spawnChildObject(jleSerializationContext& ctx)
{
    // We still want the object to be spawned initially in the scene,
    // but to immediately be moved over to the object's ownership.
    // This is because the scene will run the start() methods on the new object.
    std::shared_ptr<T> object = _containedInScene->spawnObject<T>(ctx);
    attachChildObject(object);
    return object;
}

inline std::shared_ptr<jleObject>
jleObject::spawnChildObjectFromTemplate(const jlePath &path, jleSerializationContext &ctx)
{
    auto object = _containedInScene->spawnObjectFromTemplate(path, ctx);
    attachChildObject(object);
    return object;
}

inline std::shared_ptr<jleObject>
jleObject::spawnChildObject(const std::string &objName, jleSerializationContext &ctx)
{
    auto object = _containedInScene->spawnObject<jleObject>(ctx);
    attachChildObject(object);
    return object;
}
