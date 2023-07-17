// Copyright (c) 2023. Johan Lind

#pragma once

#include "jlePath.h"
#include "jleResourceInterface.h"
#include "jleSerializedResource.h"
#include <plog/Log.h>

#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>

class jleResources
{
public:
    jleResources() = default;
    jleResources(const jleResources &) = delete;
    jleResources(jleResources &&) = delete;
    jleResources &operator=(const jleResources &) = delete;
    jleResources &operator=(jleResources &&) = delete;

    // Gets a shared_ptr to a resource from file, or a shared_ptr to an already
    // loaded copy of that resource
    template <typename T>
    std::shared_ptr<T>
    loadResourceFromFile(const jlePath &path, const bool forceReload = false)
    {
        static_assert(std::is_base_of<jleResourceInterface, T>::value, "T must derive from jleResourceInterface");

        const auto prefix = path.getPathPrefix();

        if (!forceReload) {
            auto it = _resources[prefix].find(path);
            if (it != _resources[prefix].end()) {
                if(it->second.first == typeid(T).hash_code())
                {
                    return std::static_pointer_cast<T>(it->second.second);
                }else
                {
                    LOGW << "Found another type usage from the same resource. Overwriting previous resource for: " << path;
                }
            }
        }

        std::shared_ptr<jleResourceInterface> newResource = std::make_shared<T>();

        jleLoadFromFileSuccessCode loadSuccess{jleLoadFromFileSuccessCode::FAIL};

        if constexpr (std::is_base_of<jleSerializedResource, T>::value) {
            if (newResource->getFileExtension() == path.getFileEnding()) {
                try {
                    std::ifstream i(path.getRealPath());
                    cereal::JSONInputArchive iarchive{i};
                    std::shared_ptr<jleSerializedResource> sr =
                        std::static_pointer_cast<jleSerializedResource>(newResource);
                    iarchive(sr);
                    newResource = sr;
                    loadSuccess = jleLoadFromFileSuccessCode::SUCCESS;
                } catch (std::exception &e) {
                    LOGE << "Failed loading " << path.getVirtualPath() << " - " << e.what();
                    loadSuccess = jleLoadFromFileSuccessCode::FAIL;
                }
            }
        }

        loadSuccess = newResource->loadFromFile(path);

        newResource->filepath = path.getRealPath();

        _resources[prefix].erase(path);
        if (loadSuccess == jleLoadFromFileSuccessCode::SUCCESS) {
            _resources[prefix].insert(std::make_pair(path, std::make_pair(typeid(T).hash_code(), newResource)));
        } else {
            LOGW << "Failed to load: " << path._virtualPath;
        }

        periodicResourcesCleanUp();

        return std::static_pointer_cast<T>(newResource);
    }

    void
    reloadSerializedResource(const std::shared_ptr<jleSerializedResource> &resource)
    {
        jlePath path = jlePath{resource->filepath, false};
        try {
            std::ifstream i(path.getRealPath());
            std::shared_ptr<jleSerializedResource> f = std::const_pointer_cast<jleSerializedResource>(resource);
            cereal::JSONInputArchive archive{i};
            archive(f);
            f->loadFromFile(path);

            const auto prefix = path.getPathPrefix();

            _resources[prefix].erase(path);
            _resources[prefix].insert(std::make_pair(path, std::make_pair(typeid(resource).hash_code(), resource)));
        } catch (std::exception &e) {
            LOGE << "Failed reloading serialized resource file: " << e.what();
        }
    }

    std::shared_ptr<jleSerializedResource>
    loadSerializedResourceFromFile(const jlePath &path, bool forceReload = false)
    {
        const auto prefix = path.getPathPrefix();

        std::shared_ptr<jleSerializedResource> ptr{};

        if (!forceReload) {
            auto it = _resources[prefix].find(path);
            if (it != _resources[prefix].end()) {
                return std::static_pointer_cast<jleSerializedResource>(it->second.second);
            }
        }

        try {
            std::ifstream i(path.getRealPath());
            cereal::JSONInputArchive iarchive{i};
            iarchive(ptr);

            ptr->filepath = path.getRealPath();

            if (ptr->loadFromFile(path) == jleLoadFromFileSuccessCode::FAIL) {
                LOGE << "Failed loading serialized resource's internals";
            }

            _resources[prefix].erase(path);
            _resources[prefix].insert(std::make_pair(path, std::make_pair(typeid(ptr).hash_code(), ptr)));

        } catch (std::exception &e) {
            LOGE << "Failed loading serialized resource file: " << e.what();
        }

        return ptr;
    }

    // Stores a resource with a certain path to be reused later
    template <typename T>
    void
    storeResource(std::shared_ptr<T> resource, const jlePath &path)
    {
        const auto prefix = path.getPathPrefix();

        _resources[prefix].erase(path);
        _resources[prefix].insert(std::make_pair(path, resource));

        periodicResourcesCleanUp();
    }

    // Get a resource that is already loaded
    template <typename T>
    std::shared_ptr<T>
    resource(const jlePath &path)
    {
        const auto prefix = path.getPathPrefix();
        return std::static_pointer_cast<T>(_resources[prefix].at(path));
    }

    std::shared_ptr<jleResourceInterface>
    resource(const jlePath &path)
    {
        const auto prefix = path.getPathPrefix();
        return _resources[prefix].at(path).second;
    }

    // Check to see if a resource is loaded
    bool
    isResourceLoaded(const jlePath &path)
    {
        const auto prefix = path.getPathPrefix();
        auto it = _resources[prefix].find(path);
        if (it == _resources[prefix].end()) {
            return false;
        }
        return true;
    }

    // Unload all resources from in-memory in the given drive.
    // If the resources have no other users, they will be deleted
    void
    unloadAllResources(const std::string &drive)
    {
        LOG_VERBOSE << "Unloading in-memory file resources on drive " << drive << ' ' << _resources[drive].size();
        _resources[drive].clear();
    }

    void
    unloadResource(const jlePath &path)
    {
        _resources[path.getPathPrefix()].erase(path);
    }

    using TypeHash = std::size_t;

    const std::unordered_map<std::string, std::unordered_map<jlePath, std::pair<TypeHash, std::shared_ptr<jleResourceInterface>>>> &
    resourcesMap()
    {
        return _resources;
    }

private:
    // Maps a drive like "GR:" to a resource map that contains paths such as
    // "GR:Folder/MyFile.txt" and points to the resource in memory.
    std::unordered_map<std::string, std::unordered_map<jlePath, std::pair<TypeHash, std::shared_ptr<jleResourceInterface>>>>
        _resources{};

    static inline int _periodicCleanCounter{0};

    void
    periodicResourcesCleanUp()
    {
        // Clean every 10th time that this method is called
        if (++_periodicCleanCounter % 10 == 0) {
            std::vector<jlePath> keys_for_removal;

            // TODO: Only clean up the drive that is being used, not all of them
            for (auto &&drive : _resources) {
                for (auto &&res_kvp : drive.second) {
                    // If the use count is 1, it means that no other place is
                    // the resource used other than inside the unordered map,
                    // which means that it is time to delete it from memory.
                    if (res_kvp.second.second.use_count() == 1) {
                        keys_for_removal.push_back(res_kvp.first);
                    }
                }

                for (auto &&key : keys_for_removal) {
                    drive.second.erase(key);
                }
            }
        }
    }
};
