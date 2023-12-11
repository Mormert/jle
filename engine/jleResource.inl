// Copyright (c) 2023. Johan Lind

#include "jleResource.h"

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
jleResources::loadResourceFromFile(const jlePath &path, bool forceReload)
{
    static_assert(std::is_base_of<jleResourceInterface, T>::value, "T must derive from jleResourceInterface");

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
            if (it->second.first == typeid(T).hash_code()) {
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
    if constexpr (std::is_base_of<jleSerializedOnlyResource, T>::value) {
        if (!loadSerializedResource(newResource, path)) {
            LOGE << "Failed to load serialized resource " << path.getVirtualPath();
        } else {
            loadSuccess = true;
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