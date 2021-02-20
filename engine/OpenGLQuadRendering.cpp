#include "OpenGLQuadRendering.h"

#include "3rdparty/glad/glad.h"

#include <glm/gtc/matrix_transform.hpp>

#include <string>

#include <iostream>

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

const unsigned char quadIndices[] = { 0,1,2, // first triangle (bottom left - top left - top right)
									0,2,3 }; // second triangle (bottom left - top right - bottom right)


OpenGLQuadRendering::OpenGLQuadRendering()
	: quadShader{ quadVertexShaderSrc, quadFragShaderSrc }
{

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	// x, y, u, v
	constexpr float vertices[] = {
	0.0f,	0.0f,	 0.0f,	0.0f,		// bottom left corner
	0.0f,	1.0f,	 0.0f,	1.0f,		// top left corner
	1.0f,	1.0f,	 1.0f,	1.0f,		// top right corner
	1.0f,	0.0f,	 1.0f,	0.0f };		// bottom right corner

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

OpenGLQuadRendering::~OpenGLQuadRendering()
{
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);
}

void OpenGLQuadRendering::SendTexturedQuad(TexturedQuad& texturedQuad, RenderingMethod renderingMethod)
{
	texturedQuads.push_back(texturedQuad);
}

void OpenGLQuadRendering::SendColoredQuad(ColoredQuad& coloredQuad, RenderingMethod renderingMethod)
{
	
}

void OpenGLQuadRendering::Render(iFramebuffer& framebufferOut)
{
	const int viewportWidth = framebufferOut.GetWidth();
	const int viewportHeight = framebufferOut.GetHeight();

	glm::mat4 view { glm::ortho(static_cast<float>(0.f),
		static_cast<float>(viewportWidth),
		static_cast<float>(viewportHeight),
		static_cast<float>(0.f), -1.f, 1.f) };

	framebufferOut.BindToFramebuffer();

	glClearColor(1.f, 0.f, 0.f, 1.0f); // red
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	quadShader.Use();

	for (const auto& t : texturedQuads)
	{

		if (!t.texture.IsActive())
		{
			t.texture.SetToActiveTexture();
			quadShader.SetVec2("textureDims", glm::vec2{ float(t.texture.GetWidth()), float(t.texture.GetHeight()) });
		}

		glBindVertexArray(quadVAO);

		glm::vec4 uv{ t.textureX, t.textureY, t.width, t.height };
		glm::vec3 position{ t.x, t.y, t.depth };

		quadShader.SetVec4("uv", uv);
		quadShader.SetVec3("position", position);
		quadShader.SetMat4("camera", view);
		quadShader.SetInt("texture0", 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, quadIndices);
	}

	texturedQuads.clear();

	framebufferOut.BindToDefaultFramebuffer();

}
