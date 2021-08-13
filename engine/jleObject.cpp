#include "jleObject.h"

#include "jleScene.h"

int jle::jleObject::mObjectsCreatedCount{ 0 };

jle::jleObject::jleObject()
{
	mInstanceName =  "jleObject_" + std::to_string(mObjectsCreatedCount);
	mObjectsCreatedCount++;
}

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

void jle::to_json(nlohmann::json& j, const std::shared_ptr<jleObject> o)
{
	j = nlohmann::json{
		{"obj_name", o->GetObjectNameVirtual()}
	};

	o->ToJson(j);
}

void jle::from_json(const nlohmann::json& j, std::shared_ptr<jleObject>& o)
{

}
