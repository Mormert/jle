#pragma once
#include "oRacket.h"

class oPlayerRacket : public oRacket
{
	JLE_REGISTER_OBJECT_TYPE(oPlayerRacket)

public:
	oPlayerRacket();
	virtual void Update(float dt) override;

	virtual std::string_view GetObjectNameVirtual() override
	{
		return "oPlayerRacket";
	}

	virtual void ToJson(nlohmann::json& j_out) override
	{
		oRacket::ToJson(j_out);
	}

	virtual void FromJson(const nlohmann::json& j_in) override
	{
		oRacket::FromJson(j_in);
	}
};