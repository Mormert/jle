#pragma once

#include <vector>
#include <memory>

#include "jleObject.h"
#include "jleObjectTypeUtils.h"

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
			mNewSceneObjects.push_back(newSceneObject);

			return newSceneObject;
		}

		std::shared_ptr<jleObject> SpawnObject(const std::string& objName)
		{
			auto newSceneObject = jleObjectTypeUtils::InstantiateObjectByString(objName);
			newSceneObject->mContainedInScene = this;

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
	};
}