#pragma once

#include "jleObject.h"

#include "cTransform.h"
#include "cSprite.h"

#include <memory>




#include <iostream>

#define JLE_VAR_DECL(VARTYPE, VARNAME) VARTYPE VARNAME;


class oRacket : public jle::jleObject
{
	JLE_REGISTER_OBJECT_TYPE(oRacket)
public:


	JLE_VAR_DECL(int, myInt = 5)

	virtual void SetupDefaultObject() override;

	virtual void Start() override;
	virtual void Update(float dt) override;

	virtual std::string_view GetObjectNameVirtual() override {
		return "oRacket";
	}

	virtual void ToJson(nlohmann::json& j_out) override {

		transform->ToJson(j_out["transform"]);
		sprite->ToJson(j_out["sprite"]);

		//j_out["transform"] = *transform;
		//j_out["sprite"] = *sprite;

		j_out["some_int"] = 3;
	}

	virtual void FromJson(const nlohmann::json& j_in) override {

		try {
			transform->FromJson(j_in.at("transform"));
			sprite->FromJson(j_in.at("sprite"));
		}
		catch(std::exception e){
			std::cout << e.what();
		}


		//*transform = j_in.at("transform");
		//*sprite = j_in.at("sprite");

		int a = j_in.at("some_int");
		myInt = a;
	}

protected:
	std::shared_ptr<cTransform> transform;
	std::shared_ptr<cSprite> sprite;
};