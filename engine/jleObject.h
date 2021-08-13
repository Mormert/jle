#pragma once

#include <vector>
#include <memory>

#include "jleObjectTypeUtils.h"
#include "jleComponent.h"

#include "jleJson.h"
#include "3rdparty/json.hpp"

namespace jle
{
    class jleScene;

	class jleObject : public jleJsonInterface<nlohmann::json>
	{
	public:
        std::string mInstanceName;

		virtual void Start() {}
		virtual void Update(float dt) {}

        jleObject();
        virtual ~jleObject() {}

        template <typename T>
        std::shared_ptr<T> AddComponent()
        {
            static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

            for (auto& component : mComponents)
            {
                if (std::dynamic_pointer_cast<T>(component))
                {
                    return std::dynamic_pointer_cast<T>(component);
                }
            }

            std::shared_ptr<T> newComponent = std::make_shared<T>(this, mContainedInScene);
            mComponents.push_back(newComponent);

            return newComponent;
        };

        template <typename T> std::shared_ptr<T> GetComponent()
        {
            static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

            for (auto& component : mComponents)
            {
                if (std::dynamic_pointer_cast<T>(component))
                {
                    return std::dynamic_pointer_cast<T>(component);
                }
            }

            return nullptr;
        };


        void DestroyObject();

        static const std::string_view GetObjectName()
        {
            return "jleObject"; 
        };

        virtual std::string_view GetObjectNameVirtual()
        {
            return "jleObject";
        }

        virtual void ToJson(nlohmann::json& j_out)          {}
        virtual void FromJson(const nlohmann::json& j_in)   {}

    private:
        friend class jleScene;

        jleObject(jleScene* scene);

        void StartComponents();
        void UpdateComponents(float dt);

        bool bPendingKill = false;

        static int mObjectsCreatedCount;

	protected:
        std::vector<std::shared_ptr<jleComponent>> mComponents {};

        jleScene* mContainedInScene = nullptr;

        virtual void SetupDefaultObject() {}

        friend void to_json(nlohmann::json& j, const std::shared_ptr<jleObject> o);
        friend void from_json(const nlohmann::json& j, std::shared_ptr<jleObject>& o);
	};

    void to_json(nlohmann::json& j, const std::shared_ptr<jleObject> o);
    void from_json(const nlohmann::json& j, std::shared_ptr<jleObject>& o);
}