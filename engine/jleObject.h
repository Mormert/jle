#pragma once

#include <vector>
#include <memory>

#include "jleObjectTypeUtils.h"
#include "jleComponent.h"

namespace jle
{
    class jleScene;

	class jleObject
	{
	public:

		virtual void Start() {}
		virtual void Update(float dt) {}

        jleObject() = default;
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

    private:
        friend class jleScene;

        jleObject(jleScene* scene);

        void StartComponents();
        void UpdateComponents(float dt);

        bool bPendingKill = false;

	protected:
		std::vector<std::shared_ptr<jleComponent>> mComponents;

        jleScene* mContainedInScene = nullptr;
	};
}