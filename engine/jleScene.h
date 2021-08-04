#pragma once

#include "jleObject.h"
#include "jleObjectTypeUtils.h"

#include <vector>
#include <memory>

#include "3rdparty/json.hpp"

/*namespace nlohmann {
	template <typename T>
	struct adl_serializer<std::unique_ptr<T>> {
		static void to_json(json& j, const std::unique_ptr<T>& opt) {
			if (opt.get()) {
				j = *opt;
			}
			else {
				j = nullptr;
			}
		}
	};

	template <typename T>
	struct adl_serializer<std::shared_ptr<T>> {
		static void to_json(json& j, const std::shared_ptr<T>& opt) {
			if (opt.get()) {
				j = *opt;
			}
			else {
				j = nullptr;
			}
		}
	};
}*/

namespace jle
{
	class jleScene
	{
	public:

		virtual ~jleScene() {}

		template <typename T>
		std::shared_ptr<T> SpawnObject()
		{
			static_assert(std::is_base_of<jleObject, T>::value, "T must derive from jleObject");

			auto newSceneObject = std::make_shared<T>();
			newSceneObject->mContainedInScene = this;
			newSceneObject->SetupDefaultObject();

			mNewSceneObjects.push_back(newSceneObject);

			return newSceneObject;
		}

		std::shared_ptr<jleObject> SpawnObject(const std::string& objName)
		{
			auto newSceneObject = jleObjectTypeUtils::InstantiateObjectByString(objName);
			newSceneObject->mContainedInScene = this;
			newSceneObject->SetupDefaultObject();

			mNewSceneObjects.push_back(newSceneObject);

			return newSceneObject;
		}

		void UpdateSceneObjects(float dt);
		void ProcessNewSceneObjects();

		virtual void SceneUpdate() {}
		virtual void OnSceneCreation() {}
		virtual void OnSceneDestruction() {}

		void DestroyScene();
		bool bPendingSceneDestruction = false;

	protected:
		std::vector<std::shared_ptr<jleObject>> mSceneObjects;
		std::vector<std::shared_ptr<jleObject>> mNewSceneObjects;

		friend void to_json(nlohmann::json& j, const jleScene s);
		friend void from_json(const nlohmann::json& j, jleScene& s);
	};


	void to_json(nlohmann::json& j, const jleScene s);
	void from_json(const nlohmann::json& j, jleScene& s);
}