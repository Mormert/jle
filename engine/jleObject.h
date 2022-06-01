// Copyright (c) 2022. Johan Lind

#pragma once

#include <vector>
#include <memory>

#include "jleTypeReflectionUtils.h"
#include "jleComponent.h"

#include "jleJson.h"
#include "3rdparty/json.hpp"

namespace jle {
    class jleScene;

    class jleObject : public jleJsonInterface<nlohmann::json> {
        JLE_REGISTER_OBJECT_TYPE(jleObject)
    public:
        std::string mInstanceName;

        virtual void Start() {}

        virtual void Update(float dt) {}

        jleObject();

        ~jleObject() override = default;

        template<typename T>
        std::shared_ptr<T> AddComponent() {
            static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

            for (auto &component: mComponents) {
                if (std::dynamic_pointer_cast<T>(component)) {
                    return std::dynamic_pointer_cast<T>(component);
                }
            }

            std::shared_ptr<T> newComponent = std::make_shared<T>(this, mContainedInScene);
            mComponents.push_back(newComponent);

            return newComponent;
        };

        template<typename T>
        std::shared_ptr<T> AddCustomComponent(bool start_immediate = false) {
            static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

            std::shared_ptr<T> newCustomComponent = AddComponent<T>();
            mDynamicCustomComponents.push_back(newCustomComponent);

            if (start_immediate) {
                newCustomComponent->Start();
            }

            return newCustomComponent;
        };

        std::shared_ptr<jleComponent> AddComponent(const std::string &component_name) {
            auto newComponent = jleTypeReflectionUtils::InstantiateComponentByString(component_name);
            newComponent->mAttachedToObject = this;
            newComponent->mContainedInScene = mContainedInScene;

            mComponents.push_back(newComponent);

            return newComponent;
        }

        std::shared_ptr<jleComponent>
        AddCustomComponent(const std::string &component_name, bool start_immediate = false) {
            auto newCustomComponent = AddComponent(component_name);
            mDynamicCustomComponents.push_back(newCustomComponent);

            if (start_immediate) {
                newCustomComponent->Start();
            }

            return newCustomComponent;
        }

        template<typename T>
        std::shared_ptr<T> GetComponent() {
            static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

            for (auto &component: mComponents) {
                if (std::dynamic_pointer_cast<T>(component)) {
                    return std::dynamic_pointer_cast<T>(component);
                }
            }

            return nullptr;
        };

        template<typename T>
        std::shared_ptr<T> AddDependencyComponent(const jleComponent *component) {
            static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

            std::shared_ptr<T> c = component->mAttachedToObject->GetComponent<T>();
            if (!c) {
                c = component->mAttachedToObject->AddCustomComponent<T>();
            }

            return c;
        }

        // Called from components
        void DestroyComponent(jleComponent *component);

        void DestroyObject();

        int GetComponentCount();

        virtual std::string_view GetObjectNameVirtual() {
            return "jleObject";
        }

        std::vector<std::shared_ptr<jleComponent>> &GetCustomComponents();

        void ToJson(nlohmann::json &j_out) override {}

        void FromJson(const nlohmann::json &j_in) override {}

    private:
        friend class jleScene;

        explicit jleObject(jleScene *scene);

        void StartComponents();

        void UpdateComponents(float dt);

        bool bPendingKill = false;

        static int mObjectsCreatedCount;

    protected:
        std::vector<std::shared_ptr<jleComponent>> mComponents{};

        // Dynamic Custom Components is a subset of mComponents, containing components
        // That can be added/removed dynamically, and saved on object instances in a scene.
        std::vector<std::shared_ptr<jleComponent>> mDynamicCustomComponents{};

        jleScene *mContainedInScene = nullptr;

        virtual void SetupDefaultObject() {}

        friend void to_json(nlohmann::json &j, const std::shared_ptr<jleObject> &o);

        friend void from_json(const nlohmann::json &j, std::shared_ptr<jleObject> &o);
    };

    void to_json(nlohmann::json &j, const std::shared_ptr<jleObject> &o);

    void from_json(const nlohmann::json &j, std::shared_ptr<jleObject> &o);
}