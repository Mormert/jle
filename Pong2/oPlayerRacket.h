#pragma once
#include "oRacket.h"


class oPlayerRacket : public oRacket
{
public:
	oPlayerRacket();
	virtual void Update(float dt) override;

	static const std::string_view GetObjectName()
	{
		return "oPlayerRacket";
	};

	//static jle::jleObjectTypeUtilsRegistrator<oPlayerRacket> reg;
};