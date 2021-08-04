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

void cTransform::ToJson(nlohmann::json& j_out)
{
	j_out = nlohmann::json{
		{"x", x},
		{"y", y}
	};
}

void cTransform::FromJson(const nlohmann::json& j_in)
{
	x = j_in.at("x");
	y = j_in.at("y");
}
/*
void to_json(nlohmann::json& j, const cTransform t)
{
	j = nlohmann::json{
		{"x", t.x},
		{"y", t.y}
	};
}

void from_json(const nlohmann::json& j, cTransform& t)
{
	t.x = j.at("x");
	t.y = j.at("y");
}*/
