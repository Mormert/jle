#pragma once

#include "jleObject.h"

#include "cTransform.h"
#include "cSprite.h"

#include <memory>

#define JLE_VAR_DECL(VARTYPE, VARNAME) VARTYPE VARNAME;


class oRacket : public jle::jleObject
{
	JLE_REGISTER_OBJECT_TYPE(oRacket)
public:

	oRacket();

	JLE_VAR_DECL(int, myInt = 5)

	virtual void Start() override;
	virtual void Update(float dt) override;

protected:
	std::shared_ptr<cTransform> transform;
	std::shared_ptr<cSprite> sprite;

};