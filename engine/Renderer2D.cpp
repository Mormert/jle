#include "Renderer2D.h"

#include "Shader.h"
#include "3rdparty/glad/glad.h"

#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const std::string quadVertexShaderSrc =
R"(
#version 330 core

uniform mat4 camera;

uniform vec2 textureDims;

uniform vec3 position; // world position (x, y, depth)
uniform vec4 uv; // Coords on texture (x, y, width, height)


layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec4 uvs;
out vec2 texCoords;
out vec2 texDims;

void main()
{
	gl_Position = camera * vec4(aPos * uv.zw + position.xy, position.z, 1.0);

	texCoords = aTexCoords;
	uvs = uv;
	texDims = textureDims;
}
)";


const std::string quadFragShaderSrc =
R"(
#version 330 core

uniform sampler2D texture0;

out vec4 FragColor;

in vec2 texCoords;	
in vec4 uvs;		// Texture coords (x, y, width, height)
in vec2 texDims;	

void main()
{
	vec4 col = texture(texture0, texCoords * vec2( uvs.z/texDims.x, uvs.w/texDims.y) + vec2(uvs.x/texDims.x,uvs.y/texDims.y)); 

    if(col.a < 0.1)
        discard;
    FragColor = col;
}
)";


GLubyte quadIndices[] = {	0,1,2, // first triangle (bottom left - top left - top right)
							0,2,3 }; // second triangle (bottom left - top right - bottom right)

namespace jle
{
	Renderer2D* Renderer2D::mainRenderer{ nullptr };

	void Renderer2D::Render()
	{
		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cameraMat = glm::ortho(static_cast<float>(camera.xPos),
			static_cast<float>(camera.xPos) + camera.width,
			static_cast<float>(camera.yPos) + camera.height,
			static_cast<float>(camera.yPos), -1.0f, 1.0f);

		mainRenderer = this;
	}

	void Renderer2D::SetAspectRatio(int w, int h)
	{
		glViewport(0, 0, w, h);

		float ratio = float(w) / float(h);

		camera.width = 270 * ratio;
		camera.height = 270;
	}


	Renderer2D::Renderer2D()
		: quadTextureShader{ quadVertexShaderSrc, quadFragShaderSrc },
		cameraMat{ 1.0f }
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

	glm::mat4& Renderer2D::GetCameraMat()
	{
		return cameraMat;
	}

	void Renderer2D::RenderQuadTexture(int worldX, int worldY, float depth, graphics::Texture& texture, int x, int y, int width, int height)
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


}

