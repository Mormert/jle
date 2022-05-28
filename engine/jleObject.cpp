#include "jleObject.h"

#include "jleScene.h"

#include <optional>

int jle::jleObject::mObjectsCreatedCount{0};

jle::jleObject::jleObject()
{
    mInstanceName = "jleObject_" + std::to_string(mObjectsCreatedCount);
    mObjectsCreatedCount++;
}

void jle::jleObject::DestroyComponent(jleComponent* component)
{
    for(int i = mComponents.size() - 1; i >= 0; i--)
    {
        if(mComponents[i].get() == component)
        {
            mComponents.erase(mComponents.begin() + i);
        }
    }
    for(int i = mDynamicCustomComponents.size() - 1; i >= 0; i--)
    {
        if(mDynamicCustomComponents[i].get() == component)
        {
            mDynamicCustomComponents.erase(mDynamicCustomComponents.begin() + i);
        }
    }
}

void jle::jleObject::DestroyObject()
{
    bPendingKill = true;
}

int jle::jleObject::GetComponentCount()
{
    return mComponents.size();
}

std::vector<std::shared_ptr<jle::jleComponent>>& jle::jleObject::GetCustomComponents()
{
    return mDynamicCustomComponents;
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
        {"__obj_name", o->GetObjectNameVirtual()},
        {"_instance_name", o->mInstanceName},
        {"_custom_components", o->mDynamicCustomComponents}
    };

    o->ToJson(j);
}

void jle::from_json(const nlohmann::json& j, std::shared_ptr<jleObject>& o)
{

    JLE_FROM_JSON_IF_EXISTS(j,o->mInstanceName, "_instance_name")

    for (auto&& custom_components_json : j.at("_custom_components"))
    {
        std::string componentName;
        custom_components_json.at("_comp_name").get_to(componentName);

        std::optional<std::shared_ptr<jleComponent>> existingComponent;
        for(auto&& existing_component : o->GetCustomComponents())
        {
            if(existing_component->GetComponentName() == componentName)
            {
                existingComponent = existing_component;
            }
        }

        if(existingComponent.has_value())
        {
            existingComponent->get()->FromJson(custom_components_json);
        }
        else
        {
            auto component = o->AddCustomComponent(componentName);
            component->FromJson(custom_components_json);
        }        
    }
}
