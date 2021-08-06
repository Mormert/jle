#pragma once

#include <string_view>

#include "jleJson.h"
#include "3rdparty/json.hpp"

namespace jle
{
	class jleObject;
	class jleScene;

	class jleComponent : public jleJsonInterface<nlohmann::json>
	{
	public:
		jleComponent(jleObject* owner = nullptr, jleScene* scene = nullptr);

		virtual void Start() {};
		virtual void Update(float dt) {};

		virtual const std::string_view GetComponentName() const = 0;

		virtual void ToJson(nlohmann::json& j_out)			{}
		virtual void FromJson(const nlohmann::json& j_in)	{}

	protected:
		jleObject* mAttachedToObject;
		jleScene* mContainedInScene;
	};
}
