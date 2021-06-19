#include "jleScene.h"

void jle::jleScene::UpdateSceneObjects(float dt)
{
	for (auto i = mSceneObjects.size() - 1; i >= 0; i--)
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
	if (mNewSceneObjects.size() > 0)
	{
		for (const auto& newObject : mNewSceneObjects)
		{
			newObject->Start();
			newObject->StartComponents();
		}

		mSceneObjects.assign(mNewSceneObjects.begin(), mNewSceneObjects.end());
		mNewSceneObjects.clear();
	}
}

void jle::jleScene::DestroyScene()
{
	bPendingSceneDestruction = true;
	OnSceneDestruction();
}
