// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"
#include "jlePath.h"
#include <plog/Log.h>

#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class jleResourceHolder {
public:
    // Gets a shared_ptr to a resource from file, or a shared_ptr to an already
    // loaded copy of that resource
    template <typename T>
    static std::shared_ptr<T> loadResourceFromFile(
        const jleRelativePath& path) {
        static_assert(std::is_base_of<jleFileLoadInterface, T>::value,
                      "T must derive from FileLoadInterface");

        const auto prefix = path.pathPrefix();

        auto it = _resources[prefix].find(path.relativePathStr());
        if (it != _resources[prefix].end()) {
            return std::static_pointer_cast<T>(it->second);
        }

        std::shared_ptr<T> new_resource = std::make_shared<T>();

        new_resource->loadFromFile(path.absolutePathStr());

        _resources[prefix].erase(path.relativePathStr());
        _resources[prefix].insert(
            std::make_pair(path.relativePathStr(), new_resource));

        periodicResourcesCleanUp();

        return new_resource;
    }

    // Stores a resource with a certain path to be reused later
    template <typename T>
    static void storeResource(std::shared_ptr<T> resource,
                              const jleRelativePath& path) {
        const auto prefix = path.pathPrefix();

        _resources[prefix].erase(path.relativePathStr());
        _resources[prefix].insert(
            std::make_pair(path.relativePathStr(), resource));

        periodicResourcesCleanUp();
    }

    // Get a resource that is already loaded
    template <typename T>
    static std::shared_ptr<T> resource(const jleRelativePath& path) {
        const auto prefix = path.pathPrefix();
        return std::static_pointer_cast<T>(
            _resources[prefix].at(path.relativePathStr()));
    }

    // Check to see if a resource is loaded
    static bool isResourceLoaded(const jleRelativePath& path) {
        const auto prefix = path.pathPrefix();
        auto it = _resources[prefix].find(path.relativePathStr());
        if (it == _resources[prefix].end()) {
            return false;
        }
        return true;
    }

    // Unload all resources from in-memory in the given drive.
    // If the resources have no other users, they will be deleted
    static void unloadAllResources(const std::string& drive) {
        LOG_VERBOSE << "Unloading in-memory file resources on drive " << drive
                    << ' ' << _resources[drive].size();
        _resources[drive].clear();
    }

    static void unloadResource(const jleRelativePath& path) {
        _resources[path.pathPrefix()].erase(path.relativePathStr());
    }

    static const std::unordered_map<
        std::string,
        std::unordered_map<std::string, std::shared_ptr<void>>>&
    resourcesMap() {
        return _resources;
    }

private:
    // Maps a drive like "GR:" to a resource map that contains paths such as
    // "GR:Folder/MyFile.txt" and points to the resource in memory.
    static inline std::unordered_map<
        std::string,
        std::unordered_map<std::string, std::shared_ptr<void>>>
        _resources{};

    static inline int _periodicCleanCounter{0};

    static void periodicResourcesCleanUp() {
        // Clean every 10th time that this method is called
        if (++_periodicCleanCounter % 10 == 0) {
            std::vector<std::string> keys_for_removal;

            // TODO: Only clean up the drive that is being used, not all of them
            for (auto&& drives : _resources) {
                for (auto&& res_kvp : drives.second) {
                    // If the use count is 1, it means that no other place is
                    // the resource used other than inside the unordered map,
                    // which means that it is time to delete it from memory.
                    if (res_kvp.second.use_count() == 1) {
                        keys_for_removal.push_back(res_kvp.first);
                    }
                }
            }

            for (auto&& key : keys_for_removal) {
                _resources.erase(key);
            }
        }
    }
};