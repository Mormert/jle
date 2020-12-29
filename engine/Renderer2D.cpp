#include "Renderer2D.h"

#include "Shader.h"
#include "3rdparty/glad/glad.h"

#include "Input.h"

#include <vector>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

namespace jle
{
	Renderer2D* Renderer2D::mainRenderer{ nullptr };

	void Renderer2D::RenderQuadTexture(int worldX, int worldY, float depth, graphics::Texture& texture, int x, int y, int width, int height)
	{
		pixelQuadRenderer.SendTexturedPixelQuadDynamic(worldX, worldY, depth, texture, x, y, width, height);
	}

	void Renderer2D::Render()
	{

		screenFramebuffer->PrepareForSceneRender();

		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cameraMat = glm::ortho(static_cast<float>(camera.xPos),
			static_cast<float>(camera.xPos) + camera.width,
			static_cast<float>(camera.yPos) + camera.height,
			static_cast<float>(camera.yPos), -1.0f, 1.0f);


		pixelQuadRenderer.Render(cameraMat);

		screenFramebuffer->RenderToScreen();

		mainRenderer = this;
	}

	

	void Renderer2D::SetAspectRatio(int w, int h)
	{
		screenWidth = w;
		screenHeight = h;

		glViewport(0, 0, w, h);

		float ratio = float(w) / float(h);

		camera.width = 135 * ratio;
		camera.height = 135;

		screenFramebuffer = std::make_unique<gfx::ScreenFramebuffer>(camera.width, camera.height);
	}

	int Renderer2D::GetMouseWorldX()
	{
		float widthRatio = static_cast<float>(screenWidth) / static_cast<float>(camera.width);

		int mouseXScreenSpace = Input::GetMouseX();
		int mouseXWorldSpace = static_cast<int>(static_cast<float>(mouseXScreenSpace) / widthRatio);

		return camera.xPos + mouseXWorldSpace;
	}

	int Renderer2D::GetMouseWorldY()
	{
		float widthRatio = static_cast<float>(screenHeight) / static_cast<float>(camera.height);

		int mouseYScreenSpace = Input::GetMouseY();
		int mouseYWorldSpace = static_cast<int>(static_cast<float>(mouseYScreenSpace) / widthRatio);

		return camera.yPos + mouseYWorldSpace;
	}


	Renderer2D::Renderer2D() : 
		screenFramebuffer{ std::make_unique<gfx::ScreenFramebuffer>(240, 135) },
		screenWidth{800}, screenHeight{600}
	{
		glViewport(0, 0, 800, 600);
		glEnable(GL_DEPTH_TEST); // Note : Used for 2D depth
	}


	glm::mat4& Renderer2D::GetCameraMat()
	{
		return cameraMat;
	}


}

