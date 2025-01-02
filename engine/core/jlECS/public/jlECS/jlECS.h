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

#pragma once

#include "jleBuildConfig.h"

#include <cassert>
#include <climits>

#include <algorithm>
#include <memory>
#include <vector>
#include <functional>

class jleImGuiArchive;
class jleJSONInputArchive;
class jleJSONOutputArchive;

class jleBinaryInputArchive;
class jleBinaryOutputArchive;

namespace jlECS
{

template <class T>
class ComponentNum
{
public:
    static inline uint16_t num = 0;
};

class ComponentContainer;
class ECS;
namespace Debug{
class ECS_Debug;
class Initializer;
}

constexpr const char *
getCleanTypeName(const char *name)
{
    constexpr auto startsWith = [](const char *str, const char *prefix) constexpr -> bool {
        for (std::size_t i = 0; prefix[i] != '\0'; ++i) {
            if (str[i] != prefix[i]) {
                return false;
            }
        }
        return true;
    };

    const char *result = name;

    // Skip all leading digits
    while (std::isdigit(*result)) {
        ++result;
    }

    if (startsWith(result, "class ")) {
        return result + 6;
    }
    else if (startsWith(result, "struct ")) {
        return result + 7;
    }

    return result;
}


template <class T>
inline const auto &ComponentNumV = ComponentNum<T>::num;

struct CreateCallbackData;

class ComponentContainer
{
public:
    [[nodiscard]] const char *
    getComponentTypeName()
    {
        return componentTypeName;
    }

    std::vector<uint16_t> &
    getObjectIndicesRef()
    {
        return objectIndices;
    }

    [[nodiscard]] inline size_t
    componentCount() const
    {
        return _componentCount;
    }

    inline void *
    getComponent(int componentIndex)
    {
        return getComponentF(this, componentIndex);
    }

    void
    serializeInput(jleJSONInputArchive &archive, int componentIndex)
    {
        serializeInputF_JSON(this, archive, componentIndex);
    }

    void
    serializeOutput(jleJSONOutputArchive &archive, int componentIndex)
    {
        serializeOutputF_JSON(this, archive, componentIndex);
    }

    void
    serializeInput(jleBinaryInputArchive &archive, int componentIndex)
    {
        serializeInputF_Binary(this, archive, componentIndex);
    }

    void
    serializeOutput(jleBinaryOutputArchive &archive, int componentIndex)
    {
        serializeOutputF_Binary(this, archive, componentIndex);
    }

    inline uint16_t
    addComponent(uint16_t objectIndex)
    {
        return addComponentF(this, objectIndex);
    }

    void
    allocateComponents(int count)
    {
        allocateComponentsF(this, count);
    }

    inline void
    removeComponent(int componentIndex)
    {
        removeComponentF(this, componentIndex);
    }

    template <class T>
    inline T &
    get(int32_t index)
    {
        return *reinterpret_cast<T *>(&data[index * sizeof(T)]);
    }

    template <class T>
    inline T *
    getPtr(int32_t index)
    {
        return reinterpret_cast<T *>(&data[index * sizeof(T)]);
    }

    ECS& getECS() { return *ecs; }

protected:
    friend class ECS;
    friend class Debug::ECS_Debug;
    friend class Debug::Initializer;
    friend class ComponentContainerEditor;

    template <class T>
    static uint16_t
    addComponentT(ComponentContainer *thiz, int objectIndex)
    {
        thiz->data.resize(thiz->data.size() + sizeof(T));
        auto &newComponent = thiz->get<T>(thiz->_componentCount);
        thiz->_componentCount++;

        new (&newComponent) T();

        return thiz->_componentCount - 1;
    }

    static void callAddComponentConstruct(ComponentContainer *thiz, uint16_t objectIndex, uint16_t componentIndex, void* component);

    template <class T>
    static uint16_t
    addComponentT_ConstructCallback(ComponentContainer *thiz, int objectIndex)
    {
        thiz->data.resize(thiz->data.size() + sizeof(T));
        auto &newComponent = thiz->get<T>(thiz->_componentCount);
        thiz->_componentCount++;

        new (&newComponent) T();

        uint16_t componentIndex = thiz->_componentCount - 1;

        callAddComponentConstruct(thiz, objectIndex, componentIndex, &newComponent);

        return componentIndex;
    }

    template <class T>
    static void
    allocateComponentsT(ComponentContainer *thiz, int count)
    {
        int previousCount = thiz->componentCount();
        thiz->data.resize(thiz->data.size() + sizeof(T) * count);

        for (int i = previousCount; i < count; i++) {
            auto &newComponent = thiz->get<T>(i);
            new (&newComponent) T();
        }

        thiz->_componentCount += count;
    }

    template <class T>
    static void *
    getComponentT(ComponentContainer *thiz, int componentIndex)
    {
        return thiz->getPtr<T>(componentIndex);
    }

    template <class T>
    static void
    removeComponentT(ComponentContainer *thiz, int componentIndex)
    {
        auto &last = thiz->get<T>(thiz->_componentCount - 1);
        T &toRemove = thiz->get<T>(componentIndex);

        static_assert(std::is_move_assignable<T>() && std::is_move_constructible<T>());
        toRemove.~T();
        new (&toRemove) T(std::move(last));

        thiz->data.resize(thiz->data.size() - sizeof(T));
        thiz->_componentCount--;
    }

    template <class T>
    static void
    removeComponentT_DestructCallback(ComponentContainer *thiz, int componentIndex)
    {
        auto &last = thiz->get<T>(thiz->_componentCount - 1);
        T &toRemove = thiz->get<T>(componentIndex);

        assert(thiz->onDestroyCallback);
        thiz->onDestroyCallback(reinterpret_cast<void*>(&toRemove));

        static_assert(std::is_move_assignable<T>() && std::is_move_constructible<T>());
        toRemove.~T();
        new (&toRemove) T(std::move(last));

        thiz->data.resize(thiz->data.size() - sizeof(T));
        thiz->_componentCount--;
    }

    uint16_t (*addComponentF)(ComponentContainer *, int /*objectIndex*/);
    void *(*getComponentF)(ComponentContainer *, int /*componentIndex*/);
    void (*removeComponentF)(ComponentContainer *, int /*componentIndex*/);
    void (*allocateComponentsF)(ComponentContainer *, int /*componentCount*/);

    void (*serializeInputF_JSON)(ComponentContainer *, jleJSONInputArchive &, int /*componentIndex*/);
    void (*serializeOutputF_JSON)(ComponentContainer *, jleJSONOutputArchive &, int /*componentIndex*/);
    void (*serializeInputF_Binary)(ComponentContainer *, jleBinaryInputArchive &, int /*componentIndex*/);
    void (*serializeOutputF_Binary)(ComponentContainer *, jleBinaryOutputArchive &, int /*componentIndex*/);

    std::function<void(CreateCallbackData&)> onCreateCallback = nullptr;
    std::function<void(void* /*component*/)> onDestroyCallback = nullptr;
    std::function<void(void* /*source component*/, void* /*duplicated component*/)> onDuplicateCallback = nullptr;

    // The raw component data
    std::vector<std::byte> data;

    // The indices of the components owning objects
    std::vector<uint16_t> objectIndices;

    uint16_t _componentCount = 0;
    const char *componentTypeName{};

    ECS* ecs;

    friend class ECS;
};

struct EcsObjectArray {
    // Vector containing true/false if indexed object is alive or not
    std::vector<bool> aliveObjects;

    // Vector containing component indices for each object
    // Need to take componentTypeCount into account when indexing!
    std::vector<uint16_t> componentIndices;

    // Vector containing the recycle counter for each object
    // Used to know if an ObjectRef is valid or not
    std::vector<uint16_t> objectRecycleCounter;

    // Holds indices to dead objects, that will be re-used when adding objects
    std::vector<uint16_t> freeIndices;

    uint32_t aliveObjectsCount{};
};

class ECS;

template <class T>
class ComponentRef;

namespace Debug{
class ComponentDebugBase;
}

class ObjectRef
{
public:
    ObjectRef(uint16_t objectIndex, uint16_t objectRecycleCounter, ECS *ecs);

    [[nodiscard]] inline bool isValid() const;

    [[nodiscard]] uint16_t componentCount() const;

    template <class T>
    ComponentRef<T> addComponent();

    template <class T>
    void removeComponent();

    template <class T>
    [[nodiscard]] ComponentRef<T> getComponent() const;

    template <class T>
    [[nodiscard]] T *getComponentPtr() const;

    [[nodiscard]] inline uint16_t
    objectIndex() const{ return _objectIndex; }

    [[nodiscard]] inline uint16_t
    recycleCounter() const { return _objectRecycleCounter; }

    bool
    operator==(const ObjectRef &other) const
    {
        return _objectIndex == other._objectIndex && _objectRecycleCounter == other._objectRecycleCounter &&
               ecs == other.ecs;
    }

    bool
    operator!=(const ObjectRef &other) const
    {
        return !(*this == other);
    }

    static inline void (*gObjectRefDestructFunction)(ObjectRef* objectRef);

    ~ObjectRef();

    [[nodiscard]] ECS& getECS() const { return *ecs; }

    // Can only be called from editor code
    // TODO: move this outside ObjectRef and into editor code
    std::vector<Debug::ComponentDebugBase *> *componentsDebug();
    std::vector<std::unique_ptr<Debug::ComponentDebugBase>> componentsDebug2();
    std::vector<Debug::ComponentDebugBase *> componentsDebug_;

private:
    uint16_t _objectIndex;
    uint16_t _objectRecycleCounter;

    ECS *ecs;

    template <class U>
    friend class ComponentRef;

    friend class ECS;
};

template <class T>
class ComponentRef
{
public:
    [[nodiscard]] inline bool isValid() const;

    T &operator*() const;

    T *operator->() const;

    T *get() const;

    ObjectRef getObject();

private:
    ComponentRef(uint16_t objectIndex, ECS *ecs);

    ObjectRef objectRef;

    friend class ECS;
};

struct RegisteredComponentType {
    int componentType;
    const char *componentTypeName;
};

struct CreateCallbackData{
    ObjectRef objectRef;
    uint16_t componentIndex;
    void* componentPtr;
};

struct ComponentRegistrationConfig
{
    std::function<void(CreateCallbackData&)> onCreateCallback = nullptr;
    std::function<void(void* /*component*/)> onDestroyCallback = nullptr;
    std::function<void(void* /*source component*/, void* /*duplicated component*/)> onDuplicateCallback = nullptr;

    void (*serializeInputF_JSON)(ComponentContainer *, jleJSONInputArchive&, int /*componentIndex*/)                = nullptr;
    void (*serializeOutputF_JSON)(ComponentContainer *, jleJSONOutputArchive&, int /*componentIndex*/)              = nullptr;
    void (*serializeInputF_Binary)(ComponentContainer *, jleBinaryInputArchive&, int /*componentIndex*/)            = nullptr;
    void (*serializeOutputF_Binary)(ComponentContainer *, jleBinaryOutputArchive&, int /*componentIndex*/)          = nullptr;
    void (*serializeImGuiF)(ComponentContainer *, jleImGuiArchive&, int /*componentIndex*/, int /*objectIndex*/)    = nullptr;

};

namespace Debug
{
class ComponentContainerEditor;
class Initializer{
public:
    template <class T>
    static void initializeContainerDebugT(ComponentContainerEditor& container, const ComponentRegistrationConfig& config);
};

}

class ECS
{
public:
    ECS() = default;

    ~ECS() { reset(); }

    ECS(const ECS &) = delete;

    ECS &operator=(const ECS &) = delete;

    void
    reset()
    {
        for (auto &container : componentContainers) {
            auto size = container->componentCount();
            for (int i = size - 1; i >= 0; i--) {
                container->removeComponent(i);
            }
        }
        objectArray = {};
    }

    [[nodiscard]] inline int
    aliveObjectsCount() const
    {
        return objectArray.aliveObjectsCount;
    }

    [[nodiscard]] inline int
    allocatedObjectsCount() const
    {
        return objectArray.aliveObjects.size();
    }

    template <class T>
    [[nodiscard]] inline int
    componentCount() const
    {
        int componentType = ComponentNum<T>::num;
        return componentContainers[componentType]->componentCount();
    }

    [[nodiscard]] EcsObjectArray &
    getObjectArray()
    {
        return objectArray;
    }

    virtual std::unique_ptr<ComponentContainer> createContainer()
    {
        return std::make_unique<ComponentContainer>();
    }

    template <class T>
    static void initializeContainerT(ECS* ecs, ComponentContainer& container, const ComponentRegistrationConfig& config)
    {
        constexpr int32_t allocateBytes = 10000000;
        container.data.reserve(allocateBytes);
        container.ecs = ecs;

        if(config.onCreateCallback){
            container.addComponentF = ComponentContainer::addComponentT_ConstructCallback<T>;
            container.onCreateCallback = config.onCreateCallback;
        }else{
            container.addComponentF = ComponentContainer::addComponentT<T>;
        }

        container.addComponentF = ComponentContainer::addComponentT<T>;
        container.allocateComponentsF = ComponentContainer::allocateComponentsT<T>;
        container.getComponentF = ComponentContainer::getComponentT<T>;

        if(config.onDestroyCallback){
            container.removeComponentF = ComponentContainer::removeComponentT_DestructCallback<T>;
            container.onDestroyCallback = config.onDestroyCallback;
        }else{
            container.removeComponentF = ComponentContainer::removeComponentT<T>;
        }

        if (config.onDuplicateCallback) {
            container.onDuplicateCallback = config.onDuplicateCallback;
        }else {
            if constexpr (std::is_copy_assignable_v<T>) {
                container.onDuplicateCallback = [](void* sourceComponent, void* newComponent) {
                    T& newComponentT = *static_cast<T*>(newComponent);
                    T& sourceComponentT = *static_cast<T*>(sourceComponent);
                    newComponentT = sourceComponentT;
                };
            }else {
                assert(false && "missing duplication function for given type");
            }
        }

        container.componentTypeName = getCleanTypeName(typeid(T).name());

        container.serializeInputF_JSON = config.serializeInputF_JSON;
        container.serializeOutputF_JSON = config.serializeOutputF_JSON;

        container.serializeInputF_Binary = config.serializeInputF_Binary;
        container.serializeOutputF_Binary = config.serializeOutputF_Binary;
    }

    template <class T>
    std::unique_ptr<ComponentContainer>
    createContainerT(const ComponentRegistrationConfig& config)
    {
        auto container = createContainer();
        initializeContainerT<T>(this, *container, config);
        if (isDebug()) {
            auto* debugContainer = reinterpret_cast<Debug::ComponentContainerEditor*>(container.get());
            Debug::Initializer::initializeContainerDebugT<T>(*debugContainer, config);
        }

        return container;
    }

    template <class T>
    void
    registerComponentType(const ComponentRegistrationConfig& config)
    {
        assert(ComponentNum<T>::num == 0);

        ComponentNum<T>::num = componentContainers.size();
        auto container = createContainerT<T>(config);

        componentContainers.emplace_back(std::move(container));

        RegisteredComponentType registeredComponentType{};
        registeredComponentType.componentType = ComponentNum<T>::num;
        registeredComponentType.componentTypeName = getCleanTypeName(typeid(T).name());
        registeredComponentTypeNames.push_back(registeredComponentType);

        registeredComponentTypesCount += 1;
    }

    template <class T>
    void
    registerComponentType() {
        const ComponentRegistrationConfig config{};
        registerComponentType<T>(config);
    }

    int
    getRegisteredComponentTypesCount()
    {
        return registeredComponentTypesCount;
    }

    const std::vector<RegisteredComponentType> &
    getRegisteredComponents()
    {
        return registeredComponentTypeNames;
    }

    std::vector<std::unique_ptr<ComponentContainer>> &
    getComponentContainers()
    {
        return componentContainers;
    }

    const char *
    getComponentName(int componentType)
    {
        auto it = std::find_if(registeredComponentTypeNames.begin(),
                               registeredComponentTypeNames.end(),
                               [componentType](const auto &registeredComponent) {
                                   return registeredComponent.componentType == componentType;
                               });
        if (it != registeredComponentTypeNames.end()) {
            return it->componentTypeName;
        }
        return "Unknown Component";
    }

    ObjectRef
    instantiateObject()
    {
        assert(registeredComponentTypesCount > 0);

        if (objectArray.freeIndices.empty()) {
            objectArray.componentIndices.resize(objectArray.componentIndices.size() + registeredComponentTypesCount);
            objectArray.aliveObjects.push_back(true);
            ++objectArray.aliveObjectsCount;
            objectArray.objectRecycleCounter.push_back(0);

            auto *c = &objectArray.componentIndices[objectArray.componentIndices.size() - registeredComponentTypesCount];

            for (int i = 0; i < registeredComponentTypesCount; i++) {
                c[i] = 65535;
            }

            int objectIndex = objectArray.aliveObjects.size() - 1;

            return getObject(objectIndex);
        } else {
            auto objectIndex = objectArray.freeIndices.back();
            objectArray.freeIndices.pop_back();

            objectArray.aliveObjects[objectIndex] = true;
            ++objectArray.aliveObjectsCount;
            ++objectArray.objectRecycleCounter[objectIndex];

            return getObject(objectIndex);
        }
    }

    // Used to re-instantiate an object that has been deleted, and keep the recycle counter intact to make
    // sure all ObjectRefs are still valid. Used primarily for the editor undo-redo system. Use with caution!
    void instantiateFromObjectRef(const ObjectRef& objectRef){
        int objectIndex = objectRef.objectIndex();
        assert(objectIndex >= 0 && objectIndex < objectArray.aliveObjects.size());

        assert(!objectArray.aliveObjects[objectIndex]);

        auto it = std::find(objectArray.freeIndices.begin(), objectArray.freeIndices.end(), objectIndex);
        if (it != objectArray.freeIndices.end()) {
            objectArray.freeIndices.erase(it);
        } else {
            assert(false); // ObjectRef refers to an object not properly marked as free.
        }

        objectArray.aliveObjects[objectIndex] = true;
        ++objectArray.aliveObjectsCount;
        objectArray.objectRecycleCounter[objectRef._objectIndex] = objectRef._objectRecycleCounter;

        assert(objectRef.isValid());
    }

    ObjectRef duplicateObject(const ObjectRef &originalObject)
    {
        assert(originalObject.isValid());

        ObjectRef newObject = instantiateObject();

        for (int componentType = 0; componentType < registeredComponentTypesCount; ++componentType) {
            if (void *originalComponent = getComponent(originalObject.objectIndex(), componentType)) {
                uint16_t newComponentIndex = addComponent(newObject.objectIndex(), componentType);
                void *newComponent = getComponent(newObject.objectIndex(), componentType);

                auto &container = *componentContainers[componentType];
                container.onDuplicateCallback(originalComponent, newComponent);
            }
        }

        return newObject;
    }

    void
    destroyObject(const ObjectRef &objectRef)
    {
        auto *c = &objectArray.componentIndices[objectRef._objectIndex * registeredComponentTypesCount];
        for (int componentType = 0; componentType < registeredComponentTypesCount; componentType++) {
            if (c[componentType] != 65535) {
                removeComponent(objectRef._objectIndex, componentType);
            }
        }

        objectArray.aliveObjects[objectRef._objectIndex] = false;
        --objectArray.aliveObjectsCount;
        ++objectArray.objectRecycleCounter[objectRef._objectIndex];
        objectArray.freeIndices.push_back(objectRef._objectIndex);
    }

    std::vector<ObjectRef> *
    getAllObjectsDebug()
    {
        allObjectsDebug_.clear();
        for (int i = 0; i < objectArray.aliveObjects.size(); i++) {
            if (!isObjectAlive(i)) {
                continue;
            }
            auto object = getObject(i);
            allObjectsDebug_.push_back(object);
        }
        return &allObjectsDebug_;
    }

    std::vector<ObjectRef> *
    getAllObjectsWithComponentsDebug()
    {
        allObjectsWithComponentsDebug_.clear();
        for (int i = 0; i < objectArray.aliveObjects.size(); i++) {
            if (!isObjectAlive(i)) {
                continue;
            }
            auto object = getObject(i);
            if (object.componentCount() > 0) {
                allObjectsWithComponentsDebug_.push_back(object);
            }
        }
        return &allObjectsWithComponentsDebug_;
    }

    std::vector<ObjectRef> allObjectsDebug_;
    std::vector<ObjectRef> allObjectsWithComponentsDebug_;

    [[nodiscard]] bool
    isObjectAlive(uint16_t objectIndex) const
    {
        if(objectIndex > allocatedObjectsCount()){
            return false;
        }
        return objectArray.aliveObjects[objectIndex];
    }

    ObjectRef
    getObject(uint16_t objectIndex)
    {
        assert(isObjectAlive(objectIndex));
        uint16_t recycleCounter = objectArray.objectRecycleCounter[objectIndex];
        return ObjectRef{objectIndex, recycleCounter, this};
    }

    template <class T>
    uint16_t
    addComponent(uint16_t objectIndex)
    {
        int componentType = ComponentNum<T>::num;
        return addComponent(objectIndex, componentType);
    }

    template <class T>
    void
    removeComponent(uint16_t objectIndex)
    {
        int componentType = ComponentNum<T>::num;
        removeComponent(objectIndex, componentType);
    }

    template <class T>
    ComponentRef<T>
    getComponentRef(uint16_t objectIndex)
    {
        ComponentRef<T> ref{objectIndex, this};
        return ref;
    }

    template <class T>
    T *
    getComponent(uint16_t objectIndex)
    {
        int componentType = ComponentNum<T>::num;
        return reinterpret_cast<T *>(getComponent(objectIndex, componentType));
    }

    void *
    getComponent(uint16_t objectIndex, int componentType)
    {
        auto &container = componentContainers[componentType];

        auto *c = &objectArray.componentIndices[objectIndex * registeredComponentTypesCount];
        uint16_t componentIndex = c[componentType];

        if (componentIndex == 65535) {
            return nullptr;
        }

        return container->getComponent(componentIndex);
    }

    uint16_t
    addComponent(uint16_t objectIndex, int componentType)
    {
        assert(!getComponent(objectIndex, componentType));

        auto &container = componentContainers[componentType];
        uint16_t componentIndex = container->addComponent(objectIndex);
        container->objectIndices.push_back(objectIndex);

        auto *c = &objectArray.componentIndices[objectIndex * registeredComponentTypesCount];
        c[componentType] = componentIndex;

        return componentIndex;
    }

    void
    removeComponent(uint16_t objectIndex, int componentType)
    {
        assert(getComponent(objectIndex, componentType));

        auto &container = componentContainers[componentType];

        auto *c = &objectArray.componentIndices[objectIndex * registeredComponentTypesCount];
        uint16_t componentIndex = c[componentType];

        container->removeComponent(componentIndex);

        auto oldObjectIndex = container->objectIndices.back();
        container->objectIndices[componentIndex] = oldObjectIndex;
        container->objectIndices.pop_back();

        // Make sure the object holding a reference to the component that was moved is up to date
        auto *c2 = &objectArray.componentIndices[oldObjectIndex * registeredComponentTypesCount];
        if (c2[componentType] != 65535) {
            c2[componentType] = componentIndex;
        }

        c[componentType] = 65535;
    }

    template <typename... Components>
    ObjectRef
    addObject()
    {
        auto objectRef = instantiateObject();

        (
            [&] {
                using ComponentType = Components;
                int componentType = ComponentNum<ComponentType>::num;
                addComponent(objectRef._objectIndex, componentType);
            }(),
            ...);

        return objectRef;
    }

    template <typename T>
    class Iterator
    {
    public:
        using pointer = T *;
        using reference = T &;

        explicit Iterator(std::byte *ptr, size_t index = 0) : ptr(ptr), index(index) {}

        reference
        operator*() const
        {
            return *reinterpret_cast<T *>(ptr + index * sizeof(T));
        }

        pointer
        operator->()
        {
            return reinterpret_cast<T *>(ptr + index * sizeof(T));
        }

        Iterator &
        operator++()
        {
            index++;
            return *this;
        }

        Iterator
        operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool
        operator==(const Iterator &a, const Iterator &b)
        {
            return a.index == b.index;
        }

        friend bool
        operator!=(const Iterator &a, const Iterator &b)
        {
            return a.index != b.index;
        }

    private:
        std::byte *ptr;
        size_t index;
    };

    template <typename T>
    Iterator<T>
    begin()
    {
        int componentType = ComponentNum<T>::num;
        return Iterator<T>(componentContainers[componentType]->data.data());
    }

    template <typename T>
    Iterator<T>
    end()
    {
        int componentType = ComponentNum<T>::num;
        return Iterator<T>(componentContainers[componentType]->data.data(),
                           componentContainers[componentType]->componentCount());
    }

    template <typename T>
    class ComponentRange
    {
    public:
        explicit ComponentRange(ComponentContainer &container) : container(container) {}

        Iterator<T>
        begin()
        {
            return Iterator<T>(container.data.data());
        }

        Iterator<T>
        end()
        {
            return Iterator<T>(container.data.data(), container.componentCount());
        }

    private:
        ComponentContainer &container;
    };

    template <typename T>
    ComponentRange<T>
    iterate()
    {
        int componentType = ComponentNum<T>::num;
        return ComponentRange<T>(*componentContainers[componentType]);
    }

    template <std::size_t Index, typename... Types>
    struct TypeAtIndex;

    template <std::size_t Index, typename First, typename... Rest>
    struct TypeAtIndex<Index, First, Rest...> {
        using type = typename TypeAtIndex<Index - 1, Rest...>::type;
    };

    template <typename First, typename... Rest>
    struct TypeAtIndex<0, First, Rest...> {
        using type = First;
    };

    template <std::size_t Index, typename... Types>
    using TypeAtIndex_t = typename TypeAtIndex<Index, Types...>::type;

    template <bool IncludeObjectIndex, typename... Components>
    class MultiIterator
    {
    public:
        // Using a conditional type to decide what the iterator returns.
        using value_type = std::conditional_t<
            IncludeObjectIndex,
            std::tuple<uint16_t, Components*...>,
            std::tuple<Components*...>>;

        explicit MultiIterator(ECS *ecs, size_t index, int iteratingComponentType)
            : ecs(ecs), index(index), iteratingComponentType(iteratingComponentType)
        {
            findNextComponentCombination();
        }

        value_type
        operator*() const
        {
            if constexpr (IncludeObjectIndex) {
                // Return a tuple including the object index
                return std::tuple_cat(std::make_tuple(currentObjectIndex), componentsTuple);
            } else {
                // Return a tuple with components only
                return componentsTuple;
            }
        }

        MultiIterator &
        operator++()
        {
            ++index;
            findNextComponentCombination();
            return *this;
        }

        friend bool
        operator==(const MultiIterator &a, const MultiIterator &b)
        {
            return a.index == b.index;
        }

        friend bool
        operator!=(const MultiIterator &a, const MultiIterator &b)
        {
            return a.index != b.index;
        }

    private:
        ECS *ecs;
        size_t index;
        uint16_t currentObjectIndex{0};
        int iteratingComponentType;

        std::tuple<Components *...> componentsTuple{};

        inline void
        findNextComponentCombination()
        {
            while (index < ecs->componentContainers[iteratingComponentType]->componentCount()) {
                currentObjectIndex = ecs->componentContainers[iteratingComponentType]->objectIndices[index];
                componentsTuple = std::forward_as_tuple(ecs->getComponent<Components>(currentObjectIndex)...);

                // Check if the tuple contains no null pointers
                if (std::apply([](Components *...c) { return (... && (c != nullptr)); }, componentsTuple)) {
                    // break if we found an object with all the requested components attached
                    break;
                }
                ++index;
            }
        }
    };

    template <bool IncludeObjectIndex, typename... Components>
    class MultiRange
    {
    public:
        explicit MultiRange(ECS *ecs, int iteratingOverComponentType)
            : ecs(ecs), iteratingOverComponentType(iteratingOverComponentType)
        {
        }

        MultiIterator<IncludeObjectIndex, Components...>
        begin()
        {
            return MultiIterator<IncludeObjectIndex, Components...>(ecs, 0, iteratingOverComponentType);
        }

        MultiIterator<IncludeObjectIndex, Components...>
        end()
        {
            return MultiIterator<IncludeObjectIndex, Components...>(
                ecs, ecs->componentContainers[iteratingOverComponentType]->componentCount(), iteratingOverComponentType);
        }

    private:
        ECS *ecs;
        int iteratingOverComponentType;
    };

    template <typename... Components>
    MultiRange<false, Components...>
    iterateMulti()
    {
        auto minComponentType = findMinimumComponentTypeFor<Components...>();
        return MultiRange<false, Components...>(this, minComponentType);
    }

    template <typename... Components>
    MultiRange<true, Components...> iterateMulti_IncludeObjectIndex()
    {
        auto minComponentType = findMinimumComponentTypeFor<Components...>();
        return MultiRange<true, Components...>(this, minComponentType);
    }

    // Finds the component type with the least amount of components, which will be the component
    // type that the iterator will iterate over. It minimizes the amount of required getComponent<>s.
    template <typename... Components>
    int findMinimumComponentTypeFor()
    {
        int minSize = INT_MAX;
        int minComponentType = -1;

        (
        [&] {
            using ComponentType = Components;
            int componentType = jlECS::ComponentNum<ComponentType>::num;
            int size = componentContainers[componentType]->componentCount();
            if (size < minSize) {
                minSize = size;
                minComponentType = componentType;
            }
        }(),
        ...
        );

        return minComponentType;
    }

protected:
    virtual bool isDebug() { return false; }

    std::vector<std::unique_ptr<ComponentContainer>> componentContainers;
    std::vector<RegisteredComponentType> registeredComponentTypeNames;

    EcsObjectArray objectArray;

    uint16_t registeredComponentTypesCount{};

    friend class ObjectRef;

    template <class U>
    friend class ComponentRef;
};

template <class T>
[[nodiscard]] inline bool
ComponentRef<T>::isValid() const
{
    return objectRef.isValid() && const_cast<ObjectRef &>(objectRef).getComponentPtr<T>() != nullptr;
}

template <class T>
T &
ComponentRef<T>::operator*() const
{
    return *get();
}

template <class T>
T *
ComponentRef<T>::operator->() const
{
    return get();
}

template <class T>
ObjectRef
ComponentRef<T>::getObject()
{
    return objectRef;
}

template <class T>
T *
ComponentRef<T>::get() const
{
    if (!isValid()) {
        return nullptr;
    }
    int componentType = ComponentNumV<T>;

    auto objectArray = &objectRef.ecs->objectArray;
    auto container = objectRef.ecs->componentContainers[componentType].get();

    auto *c = &objectArray->componentIndices[objectRef._objectIndex * objectRef.ecs->registeredComponentTypesCount];
    uint16_t componentIndex = c[componentType];

    return reinterpret_cast<T *>(container->getComponent(componentIndex));
}

bool
ObjectRef::isValid() const
{
    if(_objectIndex > ecs->allocatedObjectsCount()){
        return false;
    }

    return ecs->objectArray.objectRecycleCounter[_objectIndex] == _objectRecycleCounter;
}

template <class T>
ComponentRef<T>::ComponentRef(uint16_t objectIndex, ECS *ecs) : objectRef(ecs->getObject(objectIndex))
{
}

template <class T>
ComponentRef<T>
ObjectRef::addComponent()
{
    ecs->addComponent<T>(_objectIndex);
    return ecs->getComponentRef<T>(_objectIndex);
}

template <class T>
void
ObjectRef::removeComponent()
{
    ecs->removeComponent<T>(_objectIndex);
}

template <class T>
ComponentRef<T>
ObjectRef::getComponent() const {
    return ecs->getComponentRef<T>(_objectIndex);
}

template <class T>
T *
ObjectRef::getComponentPtr() const {
    return ecs->getComponent<T>(_objectIndex);
}

namespace Debug {

class ComponentDebugBase
{
public:
    virtual ~ComponentDebugBase() = default;

    virtual const char *
    getName()
    {
        return "ComponentDebugBase";
    }

    // Editor inspector serialization
    virtual void imGuiSerialize(jleImGuiArchive &ar, int objectIndex){};

    virtual void binarySerializeOut(jleBinaryOutputArchive& ar){};
    virtual void binarySerializeIn(jleBinaryInputArchive& ar){};

    void removeFromOwningObject(ObjectRef* objectRef);

    ECS *ecs{};
    ComponentContainer *container{};

    int objectIndex{};
    int componentIndex{};
    int componentType{};
};

class ComponentContainerEditor : public ComponentContainer
{
public:
    template <class T>
    class ComponentDebug : public ComponentDebugBase
    {
    public:
        T *componentPtr;

        const char *
        getName() override
        {
            return getCleanTypeName(typeid(T).name());
        }

        void
        imGuiSerialize(jleImGuiArchive &ar, int objectIndex) override
        {
            auto editorContainer = reinterpret_cast<ComponentContainerEditor*>(container);
            if(editorContainer->serializeImGuiF){
                editorContainer->serializeImGuiF(container, ar, componentIndex, objectIndex);
            }
        };

        void
        binarySerializeOut(jleBinaryOutputArchive &ar) override
        {
            auto editorContainer = reinterpret_cast<ComponentContainerEditor*>(container);
            if(editorContainer->serializeOutputF_Binary){
                editorContainer->serializeOutputF_Binary(container, ar, componentIndex);
            }
        };

        void
        binarySerializeIn(jleBinaryInputArchive &ar) override
        {
            auto editorContainer = reinterpret_cast<ComponentContainerEditor*>(container);
            if(editorContainer->serializeInputF_Binary){
                editorContainer->serializeInputF_Binary(container, ar, componentIndex);
            }
        };

    };

    class ComponentContainerDebugBase
    {
    public:
        virtual ~ComponentContainerDebugBase() = default;

        explicit ComponentContainerDebugBase(const char *typeName, ComponentContainer *cc)
            : cc(cc), componentTypeName{typeName} {};

        ComponentContainer *cc;

        const char *componentTypeName{};
        int count{0};
    };

    template <class T>
    class ComponentContainerDebug : public ComponentContainerDebugBase
    {
    public:
        explicit ComponentContainerDebug(const char *typeName, ComponentContainer *cc)
            : ComponentContainerDebugBase(typeName, cc){};

        T *debugArrayPointer{nullptr};
    };

    inline ComponentDebugBase *
    getComponentDebug(int componentIndex, int objectIndex, ECS *ecs)
    {
        return getComponentDebugF(this, componentIndex, objectIndex, ecs);
    }

    template <class T>
    static uint16_t
    addComponentT_Debug(ComponentContainer *thiz, int objectIndex)
    {
        const auto ret = ComponentContainer::addComponentT<T>(thiz, objectIndex);
        reinterpret_cast<ComponentContainerEditor*>(thiz)->debug->count = thiz->componentCount();
        return ret;
    }

    template <class T>
    static void
    allocateComponentsT_Debug(ComponentContainer *thiz, int count)
    {
        ComponentContainer::allocateComponentsT<T>(thiz, count);
        reinterpret_cast<ComponentContainerEditor*>(thiz)->debug->count = thiz->componentCount();
    }

    template <class T>
    static void
    removeComponentT_Debug(ComponentContainer *thiz, int componentIndex)
    {
        ComponentContainer::removeComponentT<T>(thiz, componentIndex);
        reinterpret_cast<ComponentContainerEditor*>(thiz)->debug->count = thiz->componentCount();
    }

    template <class T>
    static ComponentDebugBase *
    getComponentDebugT(ComponentContainer *thiz, int componentIndex, int objectIndex, ECS *ecs)
    {
        auto debug = new ComponentDebug<T>();
        debug->componentPtr = thiz->getPtr<T>(componentIndex);
        debug->ecs = ecs;
        debug->container = thiz;
        debug->objectIndex = objectIndex;
        debug->componentIndex = componentIndex;
        debug->componentType = ComponentNum<T>::num;
        return debug;
    }

private:
    friend class Debug::ECS_Debug;
    friend class Debug::Initializer;
    ComponentDebugBase *(*getComponentDebugF)(ComponentContainer *, int, int, ECS *ecs){};
    void (*serializeImGuiF)(ComponentContainer *, jleImGuiArchive&, int /*componentIndex*/, int /*objectIndex*/) = nullptr;

    std::unique_ptr<ComponentContainerDebugBase> debugSmart;
    ComponentContainerDebugBase *debug{};
};

class ECS_Debug : public ECS
{
public:
    std::unique_ptr<ComponentContainer> createContainer() override
    {
        ObjectRef::gObjectRefDestructFunction = ObjectRefDestruct;
        return std::make_unique<ComponentContainerEditor>();
    }

protected:
    bool isDebug() override{ return true; }
    static void ObjectRefDestruct(ObjectRef* objectRef);
};

template <class T>
void
Initializer::initializeContainerDebugT(ComponentContainerEditor& container, const ComponentRegistrationConfig& config)
{
    container.addComponentF = (uint16_t (*)(ComponentContainer*, int))ComponentContainerEditor::addComponentT_Debug<T>;
    container.allocateComponentsF = (void (*)(ComponentContainer*, int))ComponentContainerEditor::allocateComponentsT_Debug<T>;
    container.removeComponentF = ComponentContainerEditor::removeComponentT_Debug<T>;

    container.getComponentDebugF = ComponentContainerEditor::getComponentDebugT<T>;
    auto debug = std::make_unique<ComponentContainerEditor::ComponentContainerDebug<T>>(getCleanTypeName(typeid(T).name()), &container);
    debug->count = 0;
    debug->debugArrayPointer = reinterpret_cast<T *>(container.data.data());
    container.debug = debug.get();
    container.debugSmart = std::move(debug);

    container.serializeImGuiF = config.serializeImGuiF;
}

} // namespace Debug

namespace Serialization{

template <class T>
static void
serializeInputT_JSON(ComponentContainer *thiz, jleJSONInputArchive &archive, int componentIndex)
{
    T &ref = *thiz->getPtr<T>(componentIndex);
    archive(ref);
}

template <class T>
static void
serializeOutputT_JSON(ComponentContainer *thiz, jleJSONOutputArchive &archive, int componentIndex)
{
    T &ref = *thiz->getPtr<T>(componentIndex);
    archive(ref);
}

template <class T>
static void
serializeInputT_Binary(ComponentContainer *thiz, jleBinaryInputArchive &archive, int componentIndex)
{
    T &ref = *thiz->getPtr<T>(componentIndex);
    archive(ref);
}

template <class T>
static void
serializeOutputT_Binary(ComponentContainer *thiz, jleBinaryOutputArchive &archive, int componentIndex)
{
    T &ref = *thiz->getPtr<T>(componentIndex);
    archive(ref);
}

template <class T>
static void
serialize_ImGui(ComponentContainer *thiz, jleImGuiArchive &archive, int componentIndex, int objectIndex)
{
    T &ref = *thiz->getPtr<T>(componentIndex);
    archive(ref);
}

}

} // namespace jlECS