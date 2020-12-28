#include "PixelQuadRenderer.h"

#include "3rdparty/glad/glad.h"

namespace jle
{
	const std::string PixelQuadRenderer::quadVertexShaderSrc = 
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

	const std::string PixelQuadRenderer::quadFragShaderSrc =
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

	const unsigned char PixelQuadRenderer::quadIndices[] = { 0,1,2, // first triangle (bottom left - top left - top right)
															0,2,3 }; // second triangle (bottom left - top right - bottom right)

	PixelQuadRenderer::PixelQuadRenderer() : quadShader{ quadVertexShaderSrc, quadFragShaderSrc }
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

	void PixelQuadRenderer::SendTexturedPixelQuadDynamic(int worldX, int worldY, float depth, graphics::Texture& texture, int x, int y, int width, int height)
	{
		texturedPixelQuadsDynamic.push_back({ worldX, worldY, depth, texture, x, y, width, height });
	}


	void PixelQuadRenderer::Render(const glm::mat4 &cameraMat)
	{

		quadShader.Use();

		for (auto t : texturedPixelQuadsDynamic)
		{
			if (!t.texture.IsActive())
			{
				t.texture.SetToActiveTexture();
				quadShader.SetVec2("textureDims", glm::vec2{ float(t.texture.GetWidth()), float(t.texture.GetHeight()) });
			}

			glBindVertexArray(quadVAO);

			glm::vec4 uv{ t.x, t.y, t.width, t.height };
			glm::vec3 position{ t.worldX, t.worldY, t.depth };

			quadShader.SetVec4("uv", uv);
			quadShader.SetVec3("position", position);
			quadShader.SetMat4("camera", cameraMat);
			quadShader.SetInt("texture0", 0);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, quadIndices);
		}




		texturedPixelQuadsDynamic.clear();
	}

}