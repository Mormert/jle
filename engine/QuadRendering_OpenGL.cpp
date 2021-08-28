#include "QuadRendering_OpenGL.h"

#include "3rdparty/glad/glad.h"

#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <unordered_map>
#include <iostream>

namespace jle
{

	struct QuadData
	{
		float x, y, depth;
		float tex_x, tex_y, tex_w, tex_h;
	};

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

	const std::string quadVertexShaderSrcInstanced =
		R"(
	#version 330 core
	layout (location = 0) in vec2 aPos;
	layout (location = 1) in vec2 aTexCoords;

	layout (location = 2) in vec3 position;		// world position		(x, y, depth)
	layout (location = 3) in vec4 uv;			// Coords on texture	(tex_x, tex_y, tex_width, tex_height)

	uniform mat4 camera;
	uniform vec2 textureDims;

	//out vec3 fColor;

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

	const std::string quadFragShaderSrcInstanced =
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


	QuadRendering_OpenGL::QuadRendering_OpenGL()
		:	quadShader{ quadVertexShaderSrc, quadFragShaderSrc },
			quadShaderInstanced{ quadVertexShaderSrcInstanced, quadFragShaderSrcInstanced }
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

		
		// Setup instanced rendering
		QuadData quadDatas[100][100];
		for (int i = 0; i < 100; i++)
		{
			for (int j = 0; j < 100; j++)
			{
				quadDatas[i][j].x = 2.f + i * 8.f;
				quadDatas[i][j].y = 2.f + j * 8.f;
				quadDatas[i][j].tex_h = 8.f;
				quadDatas[i][j].tex_w = 8.f;
				quadDatas[i][j].tex_x = 0.f;
				quadDatas[i][j].tex_y = 0.f;
				quadDatas[i][j].depth = 0.f;
			}
			
		}
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData) * 10000, &quadDatas[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		/*float quadVertices[] = {
			// positions     // colors
			-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
			 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
			-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

			-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
			 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
			 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
		};*/

		glGenVertexArrays(1, &quadVAO_Instanced);
		glGenBuffers(1, &quadVBO_Instanced);
		glBindVertexArray(quadVAO_Instanced);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO_Instanced);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		// also set instance data
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
		glVertexAttribDivisor(3, 1);
		
	}

	QuadRendering_OpenGL::~QuadRendering_OpenGL()
	{
		glDeleteBuffers(1, &quadVBO);
		glDeleteVertexArrays(1, &quadVAO);

		glDeleteBuffers(1, &quadVBO_Instanced);
		glDeleteVertexArrays(1, &quadVAO_Instanced);
	}

	void QuadRendering_OpenGL::SendTexturedQuad(TexturedQuad& texturedQuad, RenderingMethod renderingMethod)
	{
		texturedQuads.push_back(texturedQuad);
	}

	void QuadRendering_OpenGL::SendColoredQuad(ColoredQuad& coloredQuad, RenderingMethod renderingMethod)
	{

	}

	void QuadRendering_OpenGL::Render(iFramebuffer& framebufferOut)
	{
		const int viewportWidth = framebufferOut.GetWidth();
		const int viewportHeight = framebufferOut.GetHeight();

		//std::cout << "framebuffer: " << viewportWidth << ", " << viewportHeight << '\n';

		glm::mat4 view{ 1.f };
		view =  glm::ortho(static_cast<float>(0.f),
			static_cast<float>(viewportWidth),
			static_cast<float>(viewportHeight),
			static_cast<float>(0.f), -1.f, 1.f);

		framebufferOut.BindToFramebuffer();

		glClearColor(0.f, 0.f, 0.f, 1.0f); // Make everything black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Change viewport dimensions to match framebuffer's dimensions
		glViewport(0, 0, viewportWidth, viewportHeight);


		std::unordered_map<std::shared_ptr<iTexture>, std::vector<QuadData>> quadDataMap;
		

		for (auto&& quad : texturedQuads)
		{
			QuadData qd;
			qd.depth = quad.depth;
			qd.tex_h = quad.height;
			qd.tex_w = quad.width;
			qd.tex_x = quad.textureX;
			qd.tex_y = quad.textureY;
			qd.x = quad.x;
			qd.y = quad.y;
			auto& vec = quadDataMap[quad.texture];
			vec.push_back(qd);
		}

		for (auto&& key : quadDataMap)
		{
			
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData) * key.second.size(), &key.second[0], GL_STATIC_DRAW);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);


			quadShaderInstanced.Use();

			if (!key.first->IsActive())
			{
				key.first->SetToActiveTexture();
				quadShaderInstanced.SetVec2("textureDims", glm::vec2{ float(key.first->GetWidth()), float(key.first->GetHeight()) });
			}

			glBindVertexArray(quadVAO_Instanced);

			quadShader.SetMat4("camera", view);
			quadShader.SetInt("texture0", 0);

			//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 10);
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, quadIndices, key.second.size());
			glBindVertexArray(0);
		}




		texturedQuads.clear();

		framebufferOut.BindToDefaultFramebuffer();

		return;

		quadShader.Use();

		for (const auto& t : texturedQuads)
		{

			if (!t.texture->IsActive())
			{
				t.texture->SetToActiveTexture();
				quadShader.SetVec2("textureDims", glm::vec2{ float(t.texture->GetWidth()), float(t.texture->GetHeight()) });
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
}


