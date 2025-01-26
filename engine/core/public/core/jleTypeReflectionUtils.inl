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

#include "jleResourceHolder.h"
#include "jleTypeReflectionUtils.h"

inline std::map<std::string, std::function<std::shared_ptr<jleObject>()>> &
jleTypeReflectionUtils::registeredObjectsRef()
{
    if (!_registeredObjectsPtr) {
        _registeredObjectsPtr = std::make_unique<std::map<std::string, std::function<std::shared_ptr<jleObject>()>>>();
    }
    return *_registeredObjectsPtr;
}

inline std::map<std::string, std::function<std::shared_ptr<jleComponent>()>> &
jleTypeReflectionUtils::registeredComponentsRef()
{
    if (!_registeredComponentsPtr) {
        _registeredComponentsPtr =
            std::make_unique<std::map<std::string, std::function<std::shared_ptr<jleComponent>()>>>();
    }
    return *_registeredComponentsPtr;
}

inline std::map<std::string, jleTypeReflectionUtils::jleRegisteredResourceInterfaceData> &
jleTypeReflectionUtils::registeredResourcesRef()
{
    if (!_registeredResourcesPtr) {
        _registeredResourcesPtr = std::make_unique<std::map<std::string, jleRegisteredResourceInterfaceData>>();
    }
    return *_registeredResourcesPtr;
}

inline std::map<std::string, std::function<std::shared_ptr<jleResourceInterface>(const jlePath &path, jleSerializationContext& ctx)>> &
jleTypeReflectionUtils::registeredFileTypeLoadersRef()
{
    if (!_registeredFileTypeLoadersPtr) {
        _registeredFileTypeLoadersPtr = std::make_unique<
            std::map<std::string, std::function<std::shared_ptr<jleResourceInterface>(const jlePath &path, jleSerializationContext& ctx)>>>();
    }
    return *_registeredFileTypeLoadersPtr;
}

template <typename T>
jleResourceTypeRegistrator<T>::jleResourceTypeRegistrator(const std::string &rName,
                                                          const std::vector<std::string> &fileExtensions)
{
    if (fileExtensions.empty()) {
        std::cerr << "No file extensions provided for resource registration of " << rName << std::endl;
    }

#ifndef NDEBUG
    std::cout << rName << " resource registered.\n";
#endif
    jleTypeReflectionUtils::jleRegisteredResourceInterfaceData data;
    data.creationFunction = []() { return std::make_shared<T>(); };
    data.fileExtensions = fileExtensions;

    jleTypeReflectionUtils::registeredResourcesRef().insert(std::make_pair(rName, data));

    for (const auto &extension : fileExtensions) {
        jleTypeReflectionUtils::registeredFileTypeLoadersRef().insert(std::make_pair(
            extension, [](const jlePath &path, jleSerializationContext& ctx) { return ctx.resources->loadResourceFromFileT<T>(path, ctx); }));
    }
}