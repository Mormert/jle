#pragma once

#include "jleObject.h"

#include "cTransform.h"
#include "cSprite.h"

#include <memory>
#include <iostream>

class oStaticSprite : public jle::jleObject
{
	JLE_REGISTER_OBJECT_TYPE(oStaticSprite)
public:

	virtual void SetupDefaultObject() override;

	virtual void Start() override;
	virtual void Update(float dt) override;

	virtual std::string_view GetObjectNameVirtual() override {
		return "oStaticSprite";
	}

	virtual void ToJson(nlohmann::json& j_out) override {

		transform->ToJson(j_out["transform"]);
		sprite->ToJson(j_out["sprite"]);
	}

	virtual void FromJson(const nlohmann::json& j_in) override {

		try {
			transform->FromJson(j_in.at("transform"));
			sprite->FromJson(j_in.at("sprite"));
		}
		catch (std::exception e) {
			std::cout << e.what();
		}
	}

	std::shared_ptr<cTransform> transform;
	std::shared_ptr<cSprite> sprite;

private:
	static inline int beginX = 0, beginY = 0;
};