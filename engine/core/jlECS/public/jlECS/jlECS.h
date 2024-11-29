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

#if JLE_BUILD_EDITOR || !defined(NDEBUG)
#define JLECS_USE_DEBUG 1
#else
#define JLECS_USE_DEBUG 0
#endif

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

constexpr const char *
getCleanTypeName(const char *name)
{
    constexpr auto startsWith = [](const char *str, const char *prefix) {
        for (std::size_t i = 0; prefix[i] != '\0'; ++i) {
            if (str[i] != prefix[i]) {
                return false;
            }
        }
        return true;
    };

    if (startsWith(name, "class ")) {
        return name + 6;
    } else if (startsWith(name, "struct ")) {
        return name + 7;
    }
    return name;
}

template <class T>
inline const auto &ComponentNumV = ComponentNum<T>::num;

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
    addComponent()
    {
        return addComponentF(this);
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

protected:
    friend class ECS;
    friend class ECSEditor;
    friend class ComponentContainerEditor;

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

    template <class T>
    static uint16_t
    addComponentT(ComponentContainer *thiz)
    {
        thiz->data.resize(thiz->data.size() + sizeof(T));
        auto &newComponent = thiz->get<T>(thiz->_componentCount);
        thiz->_componentCount++;

        new (&newComponent) T();
        return thiz->_componentCount - 1;
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
    removeComponentT(ComponentContainer *thiz, int componentIndex)
    {
        auto &last = thiz->get<T>(thiz->_componentCount - 1);

        thiz->get<T>(componentIndex) = std::move(last);
        last.~T();

        thiz->data.resize(thiz->data.size() - sizeof(T));

        thiz->_componentCount--;
    }

    uint16_t (*addComponentF)(ComponentContainer *);
    void *(*getComponentF)(ComponentContainer *, int);
    void (*removeComponentF)(ComponentContainer *, int);
    void (*allocateComponentsF)(ComponentContainer *, int);

    void (*serializeInputF_JSON)(ComponentContainer *, jleJSONInputArchive &, int);
    void (*serializeOutputF_JSON)(ComponentContainer *, jleJSONOutputArchive &, int);
    void (*serializeInputF_Binary)(ComponentContainer *, jleBinaryInputArchive &, int);
    void (*serializeOutputF_Binary)(ComponentContainer *, jleBinaryOutputArchive &, int);

    // The raw component data
    std::vector<std::byte> data;

    // The indices of the components owning objects
    std::vector<uint16_t> objectIndices;

    uint16_t _componentCount = 0;
    const char *componentTypeName{};

#if JLECS_USE_DEBUG


#endif

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

class ObjectRef
{
public:
    ObjectRef(uint16_t objectIndex, uint16_t objectRecycleCounter, ECS *ecs);

    [[nodiscard]] inline bool isValid() const;

    uint16_t componentCount();

    template <class T>
    ComponentRef<T> addComponent();

    template <class T>
    void removeComponent();

    template <class T>
    ComponentRef<T> getComponent();

    template <class T>
    T *getComponentPtr();

    [[nodiscard]] inline uint16_t
    objectIndex() const
    {
        return _objectIndex;
    }

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

#if JLECS_USE_DEBUG

    ~ObjectRef();

    std::vector<ComponentDebugBase *> *componentsDebug();

    std::vector<std::unique_ptr<ComponentDebugBase>> componentsDebug2();

    void clearComponentsDebug();

    std::vector<ComponentDebugBase *> componentsDebug_;

#endif

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
    void initializeContainerT(ComponentContainer& container)
    {
        constexpr int32_t allocateBytes = 10000000;
        container.data.reserve(allocateBytes);
        container.addComponentF = ComponentContainer::addComponentT<T>;
        container.allocateComponentsF = ComponentContainer::allocateComponentsT<T>;
        container.getComponentF = ComponentContainer::getComponentT<T>;
        container.removeComponentF = ComponentContainer::removeComponentT<T>;
        container.componentTypeName = getCleanTypeName(typeid(T).name());

        container.serializeInputF_JSON = ComponentContainer::serializeInputT_JSON<T>;
        container.serializeOutputF_JSON = ComponentContainer::serializeOutputT_JSON<T>;

        container.serializeInputF_Binary = ComponentContainer::serializeInputT_Binary<T>;
        container.serializeOutputF_Binary = ComponentContainer::serializeOutputT_Binary<T>;
    }

    template <class T>
    std::unique_ptr<ComponentContainer>
    createContainerT(int32_t allocateBytes = 10000000)
    {
        auto container = createContainer();

        return container;
    }

    template <class T>
    void
    registerComponentType()
    {
        ComponentNum<T>::num = componentContainers.size();
        componentContainers.emplace_back(createContainerT<T>());

        RegisteredComponentType registeredComponentType{};
        registeredComponentType.componentType = ComponentNum<T>::num;
        registeredComponentType.componentTypeName = getCleanTypeName(typeid(T).name());
        registeredComponentTypeNames.push_back(registeredComponentType);

        registeredComponentTypesCount += 1;
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

            auto *c =
                &objectArray.componentIndices[objectArray.componentIndices.size() - registeredComponentTypesCount];

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
        uint16_t componentIndex = container->addComponent();
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
        return ComponentRange<T>(componentContainers[componentType]);
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

    template <typename... Components>
    class MultiIterator
    {
    public:
        using value_type = std::tuple<Components *...>;

        explicit MultiIterator(ECS *ecs, size_t index, int iteratingComponentType)
            : ecs(ecs), index(index), iteratingComponentType(iteratingComponentType)
        {
            findNextComponentCombination();
        }

        value_type
        operator*() const
        {
            return componentsTuple;
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
        int iteratingComponentType;

        std::tuple<Components *...> componentsTuple{};

        inline void
        findNextComponentCombination()
        {
            while (index < ecs->componentContainers[iteratingComponentType]->componentCount()) {
                uint16_t objectIndex = ecs->componentContainers[iteratingComponentType]->objectIndices[index];
                componentsTuple = std::forward_as_tuple(ecs->getComponent<Components>(objectIndex)...);

                // Check if the tuple contains no null pointers
                if (std::apply([](Components *...c) { return (... && (c != nullptr)); }, componentsTuple)) {
                    // break if we found an object with all the requested components attached
                    break;
                }
                ++index;
            }
        }
    };

    template <typename... Components>
    class MultiRange
    {
    public:
        explicit MultiRange(ECS *ecs, int iteratingOverComponentType)
            : ecs(ecs), iteratingOverComponentType(iteratingOverComponentType)
        {
        }

        MultiIterator<Components...>
        begin()
        {
            return MultiIterator<Components...>(ecs, 0, iteratingOverComponentType);
        }

        MultiIterator<Components...>
        end()
        {
            return MultiIterator<Components...>(
                ecs, ecs->componentContainers[iteratingOverComponentType]->componentCount(), iteratingOverComponentType);
        }

    private:
        ECS *ecs;
        int iteratingOverComponentType;
    };

    template <typename... Components>
    MultiRange<Components...>
    iterateMulti()
    {
        int minSize = INT_MAX;
        int minComponentType = -1;

        // Finds the component type with the least amount of components, which will be the component
        // type that the iterator will iterate over. It minimizes the amount of required getComponent<>s.
        (
            [&] {
                using ComponentType = Components;
                int componentType = ComponentNum<ComponentType>::num;
                int size = componentContainers[componentType]->componentCount();
                if (size < minSize) {
                    minSize = size;
                    minComponentType = componentType;
                }
            }(),
            ...);

        return MultiRange<Components...>(this, minComponentType);
    }

protected:
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
    auto container = &objectRef.ecs->componentContainers[componentType];

    auto *c = &objectArray->componentIndices[objectRef._objectIndex * objectRef.ecs->registeredComponentTypesCount];
    uint16_t componentIndex = c[componentType];

    return reinterpret_cast<T *>(container->getComponent(componentIndex));
}

bool
ObjectRef::isValid() const
{
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
ObjectRef::getComponent()
{
    return ecs->getComponentRef<T>(_objectIndex);
}

template <class T>
T *
ObjectRef::getComponentPtr()
{
    return ecs->getComponent<T>(_objectIndex);
}

} // namespace jlECS