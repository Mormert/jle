#pragma once

#include "jleObject.h"
#include "jleTypeReflectionUtils.h"

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

		jleScene();
		jleScene(std::string sceneName);
		virtual ~jleScene() {}

		template <typename T>
		std::shared_ptr<T> SpawnObject()
		{
			static_assert(std::is_base_of<jleObject, T>::value, "T must derive from jleObject");

			std::shared_ptr<T> newSceneObject = std::make_shared<T>();
			ConfigurateSpawnedObject(newSceneObject);

			return newSceneObject;
		}

		std::shared_ptr<jleObject> SpawnObject(const std::string& objName)
		{
			auto newSceneObject = jleTypeReflectionUtils::InstantiateObjectByString(objName);
			ConfigurateSpawnedObject(newSceneObject);

			return newSceneObject;
		}

		void UpdateSceneObjects(float dt);
		void ProcessNewSceneObjects();

		virtual void SceneUpdate() {}
		virtual void OnSceneCreation() {}
		virtual void OnSceneDestruction() {}

		void DestroyScene();
		bool bPendingSceneDestruction = false;

		std::vector<std::shared_ptr<jleObject>>& GetSceneObjects()
		{
			return mSceneObjects;
		}

		std::string mSceneName;

	protected:
		std::vector<std::shared_ptr<jleObject>> mSceneObjects;
		std::vector<std::shared_ptr<jleObject>> mNewSceneObjects;

		friend void to_json(nlohmann::json& j, const jleScene s);
		friend void from_json(const nlohmann::json& j, jleScene& s);

	private:
		static int mScenesCreatedCount;

		inline void ConfigurateSpawnedObject(std::shared_ptr<jleObject> obj)
		{
			obj->mContainedInScene = this;
			obj->SetupDefaultObject();
			obj->mInstanceName = std::string{ obj->GetObjectNameVirtual() } + "_" + std::to_string(obj->mObjectsCreatedCount);

			mNewSceneObjects.push_back(obj);
		}
	};


	void to_json(nlohmann::json& j, const jleScene s);
	void from_json(const nlohmann::json& j, jleScene& s);
}