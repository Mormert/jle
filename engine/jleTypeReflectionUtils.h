// Copyright (c) 2022. Johan Lind

#ifndef JLETYPEREFLECTIONUTILS_H
#define JLETYPEREFLECTIONUTILS_H

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include <iostream>

#define JLE_REGISTER_OBJECT_TYPE(object_name)                                  \
    static const std::string_view GetObjectName() { return #object_name; };    \
    static inline const jleObjectTypeRegistrator<object_name> object_name_Reg{ \
        #object_name};                                                         \
                                                                               \
public:                                                                        \
    virtual std::string_view GetObjectNameVirtual() { return #object_name; }   \
                                                                               \
private:

#define JLE_REGISTER_COMPONENT_TYPE(component_name)                            \
    const std::string_view GetComponentName() const override {                 \
        return #component_name;                                                \
    }                                                                          \
    static inline const jleComponentTypeRegistrator<component_name>            \
        object_name_Reg{#component_name};

class jleObject;

class jleComponent;

class jleTypeReflectionUtils {
public:
    template <typename T>
    [[maybe_unused]] static void RegisterObject();

    template <typename T>
    [[maybe_unused]] static void RegisterComponent();

    static std::shared_ptr<jleObject> InstantiateObjectByString(
        const std::string& str);

    static std::shared_ptr<jleComponent> InstantiateComponentByString(
        const std::string& str);

    static std::map<std::string, std::function<std::shared_ptr<jleObject>()>>&
    GetRegisteredObjectsRef();

    static std::map<std::string,
                    std::function<std::shared_ptr<jleComponent>()>>&
    GetRegisteredComponentsRef();

    // Should always be accessed via GetRegisteredObjectsRef()
    static inline std::unique_ptr<
        std::map<std::string, std::function<std::shared_ptr<jleObject>()>>>
        mRegisteredObjectsPtr{nullptr};

    // Should always be accessed via GetRegisteredComponentsRef()
    static inline std::unique_ptr<
        std::map<std::string, std::function<std::shared_ptr<jleComponent>()>>>
        mRegisteredComponentsPtr{nullptr};
};

template <typename T>
class jleObjectTypeRegistrator {
public:
    explicit jleObjectTypeRegistrator(const std::string& oName);
};

template <typename T>
class jleComponentTypeRegistrator {
public:
    explicit jleComponentTypeRegistrator(const std::string& cName);
};

#include "jleTypeReflectionUtils.inl"

#endif // JLETYPEREFLECTIONUTILS_H