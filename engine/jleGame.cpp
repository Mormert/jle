#include "jleGame.h"


namespace jle
{
	void jleGame::UpdateActiveScenes(float dt)
	{
		for (auto i = mActiveScenes.size() - 1; i >= 0; i--)
		{
			if (mActiveScenes[i]->bPendingSceneDestruction)
			{
				mActiveScenes.erase(mActiveScenes.begin() + i);
				continue;
			}

			mActiveScenes[i]->SceneUpdate();
			mActiveScenes[i]->ProcessNewSceneObjects();
			mActiveScenes[i]->UpdateSceneObjects(dt);
		}
	}
}