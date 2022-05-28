#include "cSprite.h"

#include "jleObject.h"

#include "iTextureCreator.h"

#include "jleCore.h"

cSprite::cSprite(jle::jleObject* owner, jle::jleScene* scene) : jleComponent{owner, scene}, quad{nullptr}
{
}

void cSprite::CreateAndSetTextureFromPath(const std::string& path)
{
	quad.texture = jle::jleCore::core->texture_creator->LoadTextureFromPath(path);
	texturePath = path;
}

void cSprite::SetTexture(std::shared_ptr<jle::iTexture> texture)
{
	quad.texture = texture;
}

void cSprite::SetRectangleDimensions(int width, int height)
{
	quad.width = width;
	quad.height = height;
}

void cSprite::SetTextureBeginCoordinates(int x, int y)
{
	quad.textureX = x;
	quad.textureY = y;
}

void cSprite::Start()
{
	transform = mAttachedToObject->GetComponent<cTransform>();
	if (!transform)
	{
		transform = mAttachedToObject->AddCustomComponent<cTransform>();
	}

	if (texturePath != "")
	{
		CreateAndSetTextureFromPath(texturePath);
	}

}

void cSprite::Update(float dt)
{
	quad.x = transform->x;
	quad.y = transform->y;
	
	if(quad.texture.get())
	{
		jle::jleCore::core->rendering->quads->SendTexturedQuad(*&quad, RenderingMethod::Dynamic);
	}
	
	//jle::jleCore::core->rendering->texts->SendSimpleText("Hello, World", 5.f, 5.f, 1.f, 1.f, 0.7f, 1.f, 1.f);
}

const std::string_view cSprite::GetComponentName() const
{
	return "cSprite";
}

void cSprite::ToJson(nlohmann::json& j_out)
{
	j_out = nlohmann::json{
		{"path", texturePath},
		{"x", quad.x},
		{"y", quad.y},
		{"depth", quad.depth},
		{"height", quad.height},
		{"width", quad.width},
		{"textureX", quad.textureX},
		{"textureY", quad.textureY}
	};
}

void cSprite::FromJson(const nlohmann::json& j_in)
{
	texturePath = j_in.at("path");
	quad.x = j_in.at("x");
	quad.y = j_in.at("y");
	quad.depth = j_in.at("depth");
	quad.height = j_in.at("height");
	quad.width = j_in.at("width");
	quad.textureX = j_in.at("textureX");
	quad.textureY = j_in.at("textureY");

	CreateAndSetTextureFromPath(texturePath);
}