#pragma once

#include "Texture.h"
#include "Shader.h"

#include <map>
#include <glm/glm.hpp>

class Renderer2D final
{
public:

	Renderer2D();
	~Renderer2D();

	Renderer2D(const Renderer2D& r) = delete;
	Renderer2D& operator= (const Renderer2D& r) = delete;
	Renderer2D(Renderer2D&& r) = delete;
	Renderer2D& operator=(Renderer2D&& r) = delete;

	void RenderQuadTexture(int worldX, int worldY, float depth, Texture &texture, int x, int y, int width, int height);

	static Renderer2D& GetMainRenderer() { return *mainRenderer; };

private:

	unsigned int quadVBO, quadVAO;

	Shader quadTextureShader;
	
	struct {
		int xPos = 0, yPos = 0, width = 240, height = 135;
	} Camera;

	glm::mat4 cameraMat{ 1.0f };
	glm::mat4 &GetCameraMat();


	/// Called by engine
	void Render();
	void SetAspectRatio(int w, int h);
	static Renderer2D* mainRenderer;

	friend class Engine;

};

