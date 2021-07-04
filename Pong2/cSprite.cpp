#include "cSprite.h"

#include "jleObject.h"

#include "iTextureCreator.h"

#include "jleCore.h"

cSprite::cSprite(jle::jleObject* owner, jle::jleScene* scene) : jleComponent{owner, scene}, quad{nullptr}
{
}

void cSprite::CreateAndSetTextureFromPath(const std::string& path)
{
	quad.texture = jle::jleCore::core->texture_creator->CreateTextureFromImage(jle::Image{ path });
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
		transform = mAttachedToObject->AddComponent<cTransform>();
	}
}

void cSprite::Update(float dt)
{
	quad.x = transform->x;
	quad.y = transform->y;

	jle::jleCore::core->rendering->quads->SendTexturedQuad(*&quad, RenderingMethod::Dynamic);
}

const std::string_view cSprite::GetComponentName() const
{
	return "cSprite";
}
