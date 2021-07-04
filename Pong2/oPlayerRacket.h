#pragma once
#include "oRacket.h"

class oPlayerRacket : public oRacket
{
	JLE_REGISTER_OBJECT_TYPE(oPlayerRacket)

public:
	oPlayerRacket();
	virtual void Update(float dt) override;
};