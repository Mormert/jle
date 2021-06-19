#pragma once

#include <vector>
#include <memory>

#include "jleScene.h"

namespace jle
{
	class jleGame
	{
	public:
		virtual ~jleGame() {}
		virtual void Update(float dt) {}
		virtual void Start() {}

		void UpdateActiveScenes(float dt);

		template <typename T>
		std::shared_ptr<T> CreateScene()
		{
			static_assert(std::is_base_of<jleScene, T>::value, "T must derive from jleScene");

			std::shared_ptr<jleScene> newScene = std::make_shared<T>();
			mActiveScenes.push_back(newScene);

			newScene->OnSceneCreation();

			return newScene;
		}

	protected:
		std::vector<std::unique_ptr<jleScene>> mActiveScenes;
	};
}