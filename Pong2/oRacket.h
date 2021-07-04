#pragma once

#include "jleObject.h"

#include "cTransform.h"
#include "cSprite.h"

#include <memory>

class oRacket : public jle::jleObject
{
public:

	oRacket();

	virtual void Start() override;
	virtual void Update(float dt) override;

	static const std::string_view GetObjectName()
	{
		return "oRacket";
	};

protected:
	std::shared_ptr<cTransform> transform;
	std::shared_ptr<cSprite> sprite;

};

