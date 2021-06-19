#pragma once

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

	protected:
		const jleObject* mAttachedToObject;
		const jleScene* mContainedInScene;
	};
}
