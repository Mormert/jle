#pragma once

#include "jleComponent.h"

class cTransform : public jle::jleComponent
{
public:

	cTransform(jle::jleObject* owner = nullptr, jle::jleScene* scene = nullptr);

	void SetPosition(float x, float y);
	void AddPosition(float x, float y);

	float x = 0.f, y = 0.f;

	virtual const std::string_view GetComponentName() const override;
};