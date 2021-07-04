#include "jleObject.h"

#include "jleScene.h"

void jle::jleObject::DestroyObject()
{
	bPendingKill = true;
}

jle::jleObject::jleObject(jleScene* scene) : mContainedInScene{scene}
{
}

void jle::jleObject::StartComponents()
{
	for (int i = mComponents.size() - 1; i >= 0; i--)
	{
		mComponents[i]->Start();
	}
}

void jle::jleObject::UpdateComponents(float dt)
{
	for (int i = mComponents.size() - 1; i >= 0; i--)
	{
		mComponents[i]->Update(dt);
	}
}
