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

#ifndef JLE_TYPE_REFLECTION_UTILS
#define JLE_TYPE_REFLECTION_UTILS

#include "jleCommon.h"

#include "jlePath.h"
#include "jleResourceHolder.h"

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include <iostream>

// Registered by providing: ResourceName, FileExtensionAssociations (variadic)
#define JLE_REGISTER_RESOURCE_TYPE(resource_name, ...)                                                                 \
    static inline const jleResourceTypeRegistrator<resource_name> resource_name_Reg{#resource_name, {__VA_ARGS__}};    \
    virtual const std::vector<std::string> &getFileAssociations() override                                             \
    {                                                                                                                  \
        static_assert(std::is_base_of<jleResourceInterface, resource_name>::value,                                     \
                      "Resource must derive from jleResourceInterface");                                               \
        static std::vector<std::string> staticList{__VA_ARGS__};                                                       \
        return staticList;                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
public:                                                                                                                \
    static const std::vector<std::string> &getFileAssociationsStatic()                                                 \
    {                                                                                                                  \
        static_assert(std::is_base_of<jleResourceInterface, resource_name>::value,                                     \
                      "Resource must derive from jleResourceInterface");                                               \
        static std::vector<std::string> staticList{__VA_ARGS__};                                                       \
        return staticList;                                                                                             \
    }

class jleObject;
class jleComponent;
class jleResourceInterface;

class jleTypeReflectionUtils
{
public:
    struct jleRegisteredResourceInterfaceData {
        std::vector<std::string> fileExtensions;
        std::function<std::shared_ptr<jleResourceInterface>()> creationFunction;
    };

    static std::map<std::string, jleRegisteredResourceInterfaceData> &registeredResourcesRef();

    static std::map<
        std::string,
        std::function<std::shared_ptr<jleResourceInterface>(const jlePath &path, jleSerializationContext &ctx)>> &
    registeredFileTypeLoadersRef();

private:
    // Should always be accessed via registeredResourcesRef()
    static inline std::unique_ptr<std::map<std::string, jleRegisteredResourceInterfaceData>> _registeredResourcesPtr{nullptr};

    static inline std::unique_ptr<std::map<
        std::string,
        std::function<std::shared_ptr<jleResourceInterface>(const jlePath &path, jleSerializationContext &ctx)>>>
        _registeredFileTypeLoadersPtr{nullptr};
};

template <typename T>
class jleResourceTypeRegistrator
{
public:
    explicit jleResourceTypeRegistrator(const std::string &rName, const std::vector<std::string> &fileExtensions);
};

#include "jleTypeReflectionUtils.inl"

#endif // JLE_TYPE_REFLECTION_UTILS