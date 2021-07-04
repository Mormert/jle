#pragma once

#include <string_view>

namespace jle
{
	class jleObject;
	class jleScene;

	class jleComponent
	{
	public:
		jleComponent(jleObject* owner = nullptr, jleScene* scene = nullptr);

		virtual void Start() {};
		virtual void Update(float dt) {};

		virtual const std::string_view GetComponentName() const = 0;

	protected:
		jleObject* mAttachedToObject;
		jleScene* mContainedInScene;
	};
}
