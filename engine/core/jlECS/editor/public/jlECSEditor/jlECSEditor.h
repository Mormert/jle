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

#include "jlECS/jlECS.h"

namespace jlECS
{

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
    virtual void serializeComponent(jleImGuiArchive &ar){};

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
        serializeComponent(jleImGuiArchive &ar) override
        {
            componentPtr->serialize(ar);
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
    addComponentT(ComponentContainer *thiz)
    {
        const auto ret = ComponentContainer::addComponentT<T>(thiz);
        reinterpret_cast<ComponentContainerEditor*>(thiz)->debug->count = thiz->componentCount();
        return ret;
    }

    template <class T>
    static void
    allocateComponentsT(ComponentContainer *thiz, int count)
    {
        ComponentContainer::allocateComponentsT<T>();

        reinterpret_cast<ComponentContainerEditor*>(thiz)->debug->count = thiz->componentCount();
    }

    template <class T>
    static void
    removeComponentT(ComponentContainer *thiz, int componentIndex)
    {
        //ComponentContainer::removeComponentT_<T>(thiz, componentIndex);

        //reinterpret_cast<ComponentContainerEditor*>(thiz)->debug->count = thiz->componentCount();
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
    friend class ECS_Debug;
    ComponentDebugBase *(*getComponentDebugF)(ComponentContainer *, int, int, ECS *ecs){};

    std::unique_ptr<ComponentContainerDebugBase> debugSmart;
    ComponentContainerDebugBase *debug{};
};

// An extended ECS with debugging facilities.
// It's also very useful for the editor.
class ECS_Debug : public ECS
{
public:
    std::unique_ptr<ComponentContainer> createContainer() override
    {
        ObjectRef::gObjectRefDestructFunction = ObjectRefDestruct;
        return std::make_unique<ComponentContainerEditor>();
    }

    template <class T>
    void initializeContainerEditorT(ComponentContainerEditor& container)
    {
        ECS::initializeContainerT<T>(container);

        constexpr int32_t allocateBytes = 10000000;
        container.data.reserve(allocateBytes);
        container.addComponentF = ComponentContainerEditor::addComponentT<T>;
        container.allocateComponentsF = ComponentContainerEditor::allocateComponentsT<T>;
        container.getComponentF = ComponentContainer::getComponentT<T>;
        container.componentTypeName = getCleanTypeName(typeid(T).name());
        container.componentTypeId = ComponentNum<T>::num;

        // Verify that this work
        auto oldRemoveComponentF = container.removeComponentF;
        container.removeComponentF = [=](ComponentContainer *cc, int ci){
            oldRemoveComponentF(cc, ci);
            reinterpret_cast<ComponentContainerEditor*>(cc)->debug->count = cc->componentCount();
        };

        container.serializeInputF_JSON = ComponentContainer::serializeInputT_JSON<T>;
        container.serializeOutputF_JSON = ComponentContainer::serializeOutputT_JSON<T>;

        container.serializeInputF_Binary = ComponentContainer::serializeInputT_Binary<T>;
        container.serializeOutputF_Binary = ComponentContainer::serializeOutputT_Binary<T>;

        container.getComponentDebugF = ComponentContainerEditor::getComponentDebugT<T>;
        auto debug = std::make_unique<ComponentContainerEditor::ComponentContainerDebug<T>>(getCleanTypeName(typeid(T).name()), &container);
        debug->count = 0;
        debug->debugArrayPointer = reinterpret_cast<T *>(container.data.data());
        container.debug = debug.get();
        container.debugSmart = std::move(debug);
    }

private:
    static void ObjectRefDestruct(ObjectRef* objectRef);
};
}

