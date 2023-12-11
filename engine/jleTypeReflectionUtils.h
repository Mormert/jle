// Copyright (c) 2023. Johan Lind

#ifndef JLE_TYPE_REFLECTION_UTILS
#define JLE_TYPE_REFLECTION_UTILS

#include "jlePath.h"
#include "jleResource.h"

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include <iostream>

#define JLE_REGISTER_OBJECT_TYPE(object_name)                                                                          \
    static const std::string_view objectName() { return #object_name; };                                               \
    static inline const jleObjectTypeRegistrator<object_name> object_name_Reg{#object_name};                           \
                                                                                                                       \
public:                                                                                                                \
    virtual std::string_view objectNameVirtual() { return #object_name; }                                              \
    virtual std::shared_ptr<jleObject> clone() const { return std::make_shared<object_name>(*this); }                  \
                                                                                                                       \
private:

#define JLE_REGISTER_COMPONENT_TYPE(component_name)                                                                    \
    const std::string_view componentName() const override { return #component_name; }                                  \
    static inline const jleComponentTypeRegistrator<component_name> component_name_Reg{#component_name};               \
                                                                                                                       \
public:                                                                                                                \
    std::shared_ptr<jleComponent> clone() const override { return std::make_shared<component_name>(*this); }           \
    void registerSelfLua(sol::table &self) override { self[componentName()] = this; }                                  \
                                                                                                                       \
private:

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
    template <typename T>
    [[maybe_unused]] static void registerObject();

    template <typename T>
    [[maybe_unused]] static void registerComponent();

    static std::shared_ptr<jleObject> instantiateObjectByString(const std::string &str);

    static std::shared_ptr<jleComponent> instantiateComponentByString(const std::string &str);

    static std::map<std::string, std::function<std::shared_ptr<jleObject>()>> &registeredObjectsRef();

    static std::map<std::string, std::function<std::shared_ptr<jleComponent>()>> &registeredComponentsRef();

    struct jleRegisteredResourceInterfaceData {
        std::vector<std::string> fileExtensions;
        std::function<std::shared_ptr<jleResourceInterface>()> creationFunction;
    };

    static std::map<std::string, jleRegisteredResourceInterfaceData> &registeredResourcesRef();

    static std::map<std::string, std::function<std::shared_ptr<jleResourceInterface>(const jlePath &path)>> &
    registeredFileTypeLoadersRef();

private:
    // Should always be accessed via registeredObjectsRef()
    static inline std::unique_ptr<std::map<std::string, std::function<std::shared_ptr<jleObject>()>>>
        _registeredObjectsPtr{nullptr};

    // Should always be accessed via registeredComponentsRef()
    static inline std::unique_ptr<std::map<std::string, std::function<std::shared_ptr<jleComponent>()>>>
        _registeredComponentsPtr{nullptr};

    // Should always be accessed via registeredResourcesRef()
    static inline std::unique_ptr<std::map<std::string, jleRegisteredResourceInterfaceData>> _registeredResourcesPtr{
        nullptr};

    static inline std::unique_ptr<
        std::map<std::string, std::function<std::shared_ptr<jleResourceInterface>(const jlePath &path)>>>
        _registeredFileTypeLoadersPtr{nullptr};
};

template <typename T>
class jleObjectTypeRegistrator
{
public:
    explicit jleObjectTypeRegistrator(const std::string &oName);
};

template <typename T>
class jleComponentTypeRegistrator
{
public:
    explicit jleComponentTypeRegistrator(const std::string &cName);
};

template <typename T>
class jleResourceTypeRegistrator
{
public:
    explicit jleResourceTypeRegistrator(const std::string &rName, const std::vector<std::string> &fileExtensions);
};

#include "jleTypeReflectionUtils.inl"

#endif // JLE_TYPE_REFLECTION_UTILS