#include "cTransform.h"

#include "jleJson.h"

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

void cTransform::ToJson(nlohmann::json& j_out)
{
	j_out = nlohmann::json{
		{"x", x},
		{"y", y}
	};
}

void cTransform::FromJson(const nlohmann::json& j_in)
{
    JLE_FROM_JSON_WITH_DEFAULT(j_in, x, "x", 0.f);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, y, "y", 0.f);
}