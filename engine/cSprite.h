#pragma once

#include "jleComponent.h"
#include "cTransform.h"

#include "iQuadRendering.h"
#include "iTexture.h"

#include <memory>
#include <string>

class cSprite : public jle::jleComponent
{
	JLE_REGISTER_COMPONENT_TYPE(cSprite)
public:
	cSprite(jle::jleObject* owner = nullptr, jle::jleScene* scene = nullptr);

	void CreateAndSetTextureFromPath(const std::string& path);
	void SetTexture(std::shared_ptr<jle::iTexture> texture);
	void SetRectangleDimensions(int width, int height);
	void SetTextureBeginCoordinates(int x, int y);

	virtual void Start() override;
	virtual void Update(float dt) override;

	virtual const std::string_view GetComponentName() const override;

	virtual void ToJson(nlohmann::json& j_out) override;
	virtual void FromJson(const nlohmann::json& j_in) override;

private:
	std::string texturePath = "";

	TexturedQuad quad;
	std::shared_ptr<cTransform> transform;

	//friend void to_json(nlohmann::json& j, const cSprite s);
	//friend void from_json(const nlohmann::json& j, cSprite& s);
};

//void to_json(nlohmann::json& j, const cSprite s);
//void from_json(const nlohmann::json& j, cSprite& s);