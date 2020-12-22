#include "Renderer2D.h"

#include "Shader.h"
#include "3rdparty/glad/glad.h"

#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLubyte quadIndices[] = {	0,1,2, // first triangle (bottom left - top left - top right)
							0,2,3 }; // second triangle (bottom left - top right - bottom right)

Renderer2D* Renderer2D::mainRenderer{ nullptr };

void Renderer2D::Render()
{
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cameraMat = glm::ortho(static_cast<float>(Camera.xPos),
		static_cast<float>(Camera.xPos) + Camera.width,
		static_cast<float>(Camera.yPos) + Camera.height,
		static_cast<float>(Camera.yPos), -1.0f, 1.0f);

	mainRenderer = this;
}

void Renderer2D::SetAspectRatio(int w, int h)
{
	glViewport(0, 0, w, h);

	float ratio = float(w) / float(h);

	Camera.width = 270 * ratio;
	Camera.height = 270;
}


Renderer2D::Renderer2D()
	: quadTextureShader{ "../engine/shaders/quadTexture.vert", "../engine/shaders/quadTexture.frag" },
	cameraMat{1.0f}
{
	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST); // Used for "z-coordinate"

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	// x, y, u, v
	GLfloat vertices[] = {
				0.0f,	0.0f, 0.0f, 0.0f,		// bottom left corner
				0.0f,	1.0f, 0.0f, 1.0f,		// top left corner
				1.0f,	1.0f, 1.0f,	1.0f,		// top right corner
				1.0f,	0.0f, 1.0f,	0.0f };		// bottom right corner

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex position attribute x, y
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coords attribute u, v
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Renderer2D::~Renderer2D()
{
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);
}

glm::mat4 &Renderer2D::GetCameraMat()
{
	return cameraMat;
}

void Renderer2D::RenderQuadTexture(int worldX, int worldY, float depth , Texture &texture, int x, int y, int width, int height)
{
	quadTextureShader.Use();

	if (!texture.IsActive())
	{
		texture.SetToActiveTexture();
		quadTextureShader.SetVec2("textureDims", glm::vec2{ float(texture.GetWidth()), float(texture.GetHeight()) });
	}
	
	glBindVertexArray(quadVAO);

	glm::vec4 uv{ x, y, width, height };
	glm::vec3 position{ worldX, worldY, depth };

	quadTextureShader.SetVec4("uv", uv);
	quadTextureShader.SetVec3("position", position);
	quadTextureShader.SetMat4("camera", GetCameraMat());
	quadTextureShader.SetInt("texture0", 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, quadIndices);
}