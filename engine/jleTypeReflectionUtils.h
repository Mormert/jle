// Copyright (c) 2023. Johan Lind

#pragma once

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
                                                                                                                       \
private:

#define JLE_REGISTER_RESOURCE_TYPE(resource_name, filename_extension)                                                  \
    static inline const jleResourceTypeRegistrator<resource_name> resource_name_Reg{#resource_name,                    \
                                                                                    #filename_extension};              \
    virtual std::string getFileExtension()                                                                             \
    {                                                                                                                  \
        static_assert(std::is_base_of<jleResourceInterface, resource_name>::value,                                     \
                      "Resource must derive from jleResourceInterface");                                               \
        return #filename_extension;                                                                                    \
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
        std::string filenameExtension;
        std::function<std::shared_ptr<jleResourceInterface>()> creationFunction;
    };

    static std::map<std::string, jleRegisteredResourceInterfaceData> &registeredResourcesRef();

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
    explicit jleResourceTypeRegistrator(const std::string &rName, const std::string &fileExtension);
};

#include "jleTypeReflectionUtils.inl"
