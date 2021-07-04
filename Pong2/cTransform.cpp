#include "cTransform.h"

cTransform::cTransform(jle::jleObject* owner, jle::jleScene* scene) : jleComponent{owner, scene}
{
}

void cTransform::SetPosition(float x, float y)
{
	this->x = x;
	this->y = y;
}

void cTransform::AddPosition(float x, float y)
{
	this->x += x;
	this->y += y;
}

const std::string_view cTransform::GetComponentName() const
{
	return "cTransform";
}
