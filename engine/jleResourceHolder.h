// Copyright (c) 2022. Johan Lind

#pragma once

#include "FileLoadInterface.h"
#include "jlePath.h"
#include <plog/Log.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <future>
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>

class jleResourceHolder {
public:

    // Gets a shared_ptr to a resource from file, or a shared_ptr to an already loaded copy of that resource
    template<typename T>
    static std::shared_ptr<T> LoadResourceFromFile(const jleRelativePath &path) {
        static_assert(std::is_base_of<FileLoadInterface, T>::value, "T must derive from FileLoadInterface");

        const auto prefix = path.GetPathPrefix();

        auto it = mResources[prefix].find(path.GetRelativePathStr());
        if (it != mResources[prefix].end()) {
            return std::static_pointer_cast<T>(it->second);
        }

        std::shared_ptr<T> new_resource = std::make_shared<T>();

        new_resource->LoadFromFile(path.GetAbsolutePathStr());

        mResources[prefix].erase(path.GetRelativePathStr());
        mResources[prefix].insert(std::make_pair(path.GetRelativePathStr(), new_resource));

        PeriodicResourcesCleanUp();

        return new_resource;
    }

    // Stores a resource with a certain path to be reused later
    template<typename T>
    static void StoreResource(std::shared_ptr<T> resource, const jleRelativePath &path) {
        const auto prefix = path.GetPathPrefix();

        mResources[prefix].erase(path.GetRelativePathStr());
        mResources[prefix].insert(std::make_pair(path.GetRelativePathStr(), resource));

        PeriodicResourcesCleanUp();
    }

    // Get a resource that is already loaded
    template<typename T>
    static std::shared_ptr<T> GetResource(const jleRelativePath &path) {
        const auto prefix = path.GetPathPrefix();
        return std::static_pointer_cast<T>(mResources[prefix].at(path.GetRelativePathStr()));
    }

    // Check to see if a resource is loaded
    static bool IsResourceLoaded(const jleRelativePath &path) {
        const auto prefix = path.GetPathPrefix();
        auto it = mResources[prefix].find(path.GetRelativePathStr());
        if (it == mResources[prefix].end()) {
            return false;
        }
        return true;
    }

    // Unload all resources from in-memory in the given drive.
    // If the resources have no other users, they will be deleted
    static void UnloadAllResources(const std::string &drive) {
        LOG_VERBOSE << "Unloading in-memory file resources on drive " << drive << ' ' << mResources[drive].size();
        mResources[drive].clear();
    }

    static void UnloadResource(const jleRelativePath& path)
    {
        mResources[path.GetPathPrefix()].erase(path.GetRelativePathStr());
    }

    static const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<void>>> &
    GetResourcesMap() {
        return mResources;
    }

private:

    // Maps a drive like "GR:" to a resource map that contains paths such as "GR:Folder/MyFile.txt"
    // and points to the resource in memory.
    static inline std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<void>>> mResources{};

    static inline int mPeriodicCleanCounter{0};

    static void PeriodicResourcesCleanUp() {
        // Clean every 10th time that this method is called
        if (++mPeriodicCleanCounter % 10 == 0) {
            std::vector<std::string> keys_for_removal;

            // TODO: Only clean up the drive that is being used, not all of them
            for (auto &&drives: mResources) {
                for (auto &&res_kvp: drives.second) {
                    // If the use count is 1, it means that no other place is the resource used other
                    // than inside the unordered map, which means that it is time to delete it from memory.
                    if (res_kvp.second.use_count() == 1) {
                        keys_for_removal.push_back(res_kvp.first);
                    }
                }
            }

            for (auto &&key: keys_for_removal) {
                mResources.erase(key);
            }
        }
    }
};