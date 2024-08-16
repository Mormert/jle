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

#ifndef JLE_RESOURCE
#define JLE_RESOURCE

#include "core/jleCommon.h"

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
#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>

using jleResourceHolder =
    std::unordered_map<std::string,
                       std::unordered_map<jlePath, std::pair<std::size_t, std::shared_ptr<jleResourceInterface>>>>;

template <typename T>
struct jleResourceRef;

class jleResources
{
public:
    jleResources() = default;
    jleResources(const jleResources &) = delete;
    jleResources(jleResources &&) = delete;
    jleResources &operator=(const jleResources &) = delete;
    jleResources &operator=(jleResources &&) = delete;

    static std::shared_ptr<jleResourceInterface> loadResourceFromFile(const jlePath &path);

    // Gets a shared_ptr to a resource from file, or a shared_ptr to an already loaded copy of that resource
    template <typename T>
    std::shared_ptr<T> loadResourceFromFile(const jlePath &path, bool forceReload = false);

    void reloadSerializedResource(const std::shared_ptr<jleSerializedResource> &resource);

    std::shared_ptr<jleSerializedResource> loadSerializedResourceFromFile(const jlePath &path,
                                                                          bool forceReload = false);

    // Get a resource that is already loaded
    template <typename T>
    std::shared_ptr<T> getResource(const jlePath &path);

    std::shared_ptr<jleResourceInterface> getResource(const jlePath &path);

    void storeResource(const std::shared_ptr<jleResourceInterface>& resource, const jlePath& path);

    template <typename T>
    jleResourceRef<T> storeResource(const std::shared_ptr<T>& resource, const jlePath& path);

    // Check to see if a resource is loaded
    bool isResourceLoaded(const jlePath &path);

    // Unload all resources from in-memory in the given drive.
    // If the resources have no other users, they will be deleted
    void unloadAllResources(const std::string &drive);

    void unloadResource(const jlePath &path);

    const jleResourceHolder &resourcesMap();

private:
    // Maps a drive like "GR:" to a resource map that contains paths such as
    // "GR:Folder/MyFile.txt" and points to the resource in memory.
    jleResourceHolder _resources{};

    static inline int _periodicCleanCounter{0};

    template <typename T>
    static bool checkFileEndingMatchResourceType(const jlePath& path);

    static bool loadSerializedResource(std::shared_ptr<jleResourceInterface>& resource, const jlePath& path);

    void periodicResourcesCleanUp();
};

#include "jleResource.inl"

#endif // JLE_RESOURCE