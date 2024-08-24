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

#include "jleResource.h"

#include "jleResourceRef.h"

template <typename T>
bool
jleResources::checkFileEndingMatchResourceType(const jlePath &path)
{
    static_assert(std::is_base_of<jleResourceInterface, T>::value, "T must derive from jleResourceInterface");
    const auto fileEnding = path.getFileEnding();
    const auto &possibleFileEndingsForThisType = T::getFileAssociationsStatic();
    for (auto &possibleFileEnding : possibleFileEndingsForThisType) {
        if (fileEnding == possibleFileEnding) {
            return true;
        }
    }
    return false;
}

template <typename T>
std::shared_ptr<T>
jleResources::loadResourceFromFile(const jlePath &path, jleSerializationContext ctx, bool forceReload)
{
    static_assert(std::is_base_of<jleResourceInterface, T>::value, "T must derive from jleResourceInterface");

    ctx.resources = this;

    if (!checkFileEndingMatchResourceType<T>(path)) {
        const auto &possibleFileEndingsForThisType = T::getFileAssociationsStatic();
        std::string possibleEndings = " ";
        for (const auto &ending : possibleFileEndingsForThisType) {
            possibleEndings += '.' + ending + " ";
        }
        LOGE << "Resource " << path.getVirtualPath() << " of type '" << typeid(T).name()
             << "' can't be loaded with provided path extension: '" << path.getFileEnding()
             << "'. Possible endings are: {" << possibleEndings << '}';

        return nullptr;
    }

    const auto virtualDrive = path.getPathVirtualDrive();

    if (!forceReload) {
        auto it = _resources[virtualDrive].find(path);
        if (it != _resources[virtualDrive].end()) {
            const auto hashCode = typeid(T).hash_code();
            if (it->second.first == hashCode) {
                return std::static_pointer_cast<T>(it->second.second);
            } else {
                LOGW << "Found another type usage from the same resource. Overwriting previous resource for: "
                     << path.getVirtualPath();
            }
        }
    }

    std::shared_ptr<jleResourceInterface> newResource = std::make_shared<T>();
    newResource->path = path;

    bool loadSuccess = false;
    if constexpr (std::is_base_of<jleSerializedResource, T>::value) {
        if (newResource->getPrimaryFileAssociation() == path.getFileEnding()) {
            if (!loadSerializedResource(newResource, path, ctx)) {
                LOGE << "Failed to load serialized resource " << path.getVirtualPath();
            } else {
                loadSuccess = true;
            }
        }

        if constexpr (!std::is_base_of<jleSerializedOnlyResource, T>::value) {
            loadSuccess = newResource->loadFromFile(path);
        }
    } else {
        loadSuccess = newResource->loadFromFile(path);
    }

    _resources[virtualDrive].erase(path);
    if (loadSuccess) {
        _resources[virtualDrive].insert(std::make_pair(path, std::make_pair(typeid(T).hash_code(), newResource)));
    } else {
        LOGE << "Failed to load: " << path._virtualPath;
    }

    periodicResourcesCleanUp();

    return std::static_pointer_cast<T>(newResource);
}

template <typename T>
std::shared_ptr<T>
jleResources::getResource(const jlePath &path)
{
    const auto prefix = path.getPathVirtualDrive();
    return std::static_pointer_cast<T>(_resources[prefix].at(path));
}

template <typename T>
jleResourceRef<T>
jleResources::storeResource(const std::shared_ptr<T> &resource, const jlePath &path)
{
    const auto prefix = path.getPathVirtualDrive();
    const auto hashCode = typeid(T).hash_code();

    _resources[prefix].insert(std::make_pair(path, std::make_pair(hashCode, resource)));
    return jleResourceRef<T>{path};
}