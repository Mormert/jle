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
#include "jleTypeReflectionUtils.h"

std::shared_ptr<jleResourceInterface>
jleResources::loadResourceFromFile(const jlePath &path)
{
    const auto &typeLoaders = jleTypeReflectionUtils::registeredFileTypeLoadersRef();
    const auto fileEnding = path.getFileEnding();
    auto it = typeLoaders.find(fileEnding);
    if (it != typeLoaders.end()) {
        return it->second(path);
    } else {
        LOGE << "Failed to load non-registered file type with extension " << fileEnding;
        return nullptr;
    }
}

void
jleResources::reloadSerializedResource(const std::shared_ptr<jleSerializedResource> &resource)
{
    jlePath path = resource->path;
    try {
        std::ifstream i(path.getRealPath());
        std::shared_ptr<jleSerializedResource> f = std::const_pointer_cast<jleSerializedResource>(resource);
        cereal::JSONInputArchive archive{i};
        archive(f);
        if (!f->loadFromFile(path)) {
            LOGE << "Failed reloading serialized resource file: " << resource->path.getVirtualPath();
        }

        const auto prefix = path.getPathVirtualDrive();

        _resources[prefix].erase(path);
        _resources[prefix].insert(std::make_pair(path, std::make_pair(typeid(resource).hash_code(), resource)));
    } catch (std::exception &e) {
        LOGE << "Failed reloading serialized resource file: " << e.what();
    }
}

std::shared_ptr<jleSerializedResource>
jleResources::loadSerializedResourceFromFile(const jlePath &path, bool forceReload)
{
    const auto prefix = path.getPathVirtualDrive();

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

        ptr->path = path;

        if (ptr->loadFromFile(path) == false) {
            LOGE << "Failed loading serialized resource's internals";
        }

        _resources[prefix].erase(path);
        _resources[prefix].insert(std::make_pair(path, std::make_pair(typeid(ptr).hash_code(), ptr)));

    } catch (std::exception &e) {
        LOGE << "Failed loading serialized resource file: " << e.what();
    }

    return ptr;
}

bool
jleResources::isResourceLoaded(const jlePath &path)
{
    const auto prefix = path.getPathVirtualDrive();
    auto it = _resources[prefix].find(path);
    if (it == _resources[prefix].end()) {
        return false;
    }
    return true;
}

void
jleResources::unloadAllResources(const std::string &drive)
{
    LOG_VERBOSE << "Unloading in-memory file resources on drive " << drive << ' ' << _resources[drive].size();
    _resources[drive].clear();
}

void
jleResources::unloadResource(const jlePath &path)
{
    _resources[path.getPathVirtualDrive()].erase(path);
}

const std::unordered_map<std::string,
                         std::unordered_map<jlePath, std::pair<std::size_t, std::shared_ptr<jleResourceInterface>>>> &
jleResources::resourcesMap()
{
    return _resources;
}

void
jleResources::periodicResourcesCleanUp()
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

std::shared_ptr<jleResourceInterface>
jleResources::getResource(const jlePath &path)
{
    const auto prefix = path.getPathVirtualDrive();
    return _resources[prefix].at(path).second;
}

void
jleResources::storeResource(const std::shared_ptr<jleResourceInterface>& resource, const jlePath &path)
{
    const auto prefix = path.getPathVirtualDrive();
    _resources[prefix].insert(std::make_pair(path, std::make_pair(typeid(resource).hash_code(), resource)));
}

bool
jleResources::loadSerializedResource(std::shared_ptr<jleResourceInterface> &resource, const jlePath &path)
{
    if (resource->getPrimaryFileAssociation() == path.getFileEnding()) {
        try {
            std::ifstream i(path.getRealPath());
            cereal::JSONInputArchive iarchive{i};
            std::shared_ptr<jleSerializedResource> sr = std::static_pointer_cast<jleSerializedResource>(resource);
            iarchive(sr);
            resource = sr;
            resource->path = path;
            return true;
        } catch (std::exception &e) {
            LOGE << e.what();
            return false;
        }
    }
    return false;
}
