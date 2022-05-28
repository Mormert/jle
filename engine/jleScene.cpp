#include "jleScene.h"

#include <iostream>

int jle::jleScene::mScenesCreatedCount{ 0 };

jle::jleScene::jleScene()
{
	mSceneName = "Scene_" + std::to_string(mScenesCreatedCount);
	mScenesCreatedCount++;
}

jle::jleScene::jleScene(const std::string& sceneName)
{
	this->mSceneName = sceneName;
	mScenesCreatedCount++;
}

void jle::jleScene::UpdateSceneObjects(float dt)
{
	for (int32_t i = mSceneObjects.size() - 1; i >= 0; i--)
	{
		if (mSceneObjects[i]->bPendingKill)
		{
			mSceneObjects.erase(mSceneObjects.begin() + i);
			continue;
		}

		mSceneObjects[i]->Update(dt);
		mSceneObjects[i]->UpdateComponents(dt);
	}
}

void jle::jleScene::ProcessNewSceneObjects()
{
	if (!mNewSceneObjects.empty())
	{
		for (const auto& newObject : mNewSceneObjects)
		{
			newObject->Start();
			newObject->StartComponents();
		}

		mSceneObjects.insert(std::end(mSceneObjects), std::begin(mNewSceneObjects), std::end(mNewSceneObjects));
		mNewSceneObjects.clear();
	}
}

void jle::jleScene::DestroyScene()
{
	bPendingSceneDestruction = true;

	mSceneObjects.clear();
	mNewSceneObjects.clear();

	OnSceneDestruction();
}

void jle::to_json(nlohmann::json& j, const jleScene& s)
{
	j = nlohmann::json{
		{"objects", s.mSceneObjects},
        {"sceneName", s.mSceneName}
	};
}

void jle::from_json(const nlohmann::json& j, jleScene& s)
{
    JLE_FROM_JSON_IF_EXISTS(j, s.mSceneName, "sceneName");
    JLE_FROM_JSON_IF_EXISTS(j, s.mSceneName, "sceneName");

	for (auto object_json : j.at("objects"))
	{
		std::string objectsName;
		object_json.at("__obj_name").get_to(objectsName);
		std::cout << objectsName;

		auto spawnedObjFromJson = s.SpawnObject(objectsName);
		jle::from_json(object_json, spawnedObjFromJson);
		spawnedObjFromJson->FromJson(object_json);

	}
}
