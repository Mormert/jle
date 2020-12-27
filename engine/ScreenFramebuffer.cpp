#include "ScreenFramebuffer.h"

#include "3rdparty/glad/glad.h"

#include "Window.h"

// Based on and inspired by examples found on learnopengl.com

namespace jle
{
	namespace gfx
	{

		const std::string quadScreenShaderVertexSource =
		R"(
			#version 330 core
			layout (location = 0) in vec2 aPos;
			layout (location = 1) in vec2 aTexCoords;

			out vec2 TexCoords;

			void main()
			{
				gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
				TexCoords = aTexCoords;
			}
		)";

		const std::string quadScreenShaderFragSource =
		R"(
			#version 330 core
			out vec4 FragColor;
  
			in vec2 TexCoords;

			uniform sampler2D screenTexture;

			void main()
			{ 
				FragColor = texture(screenTexture, TexCoords);
			}
		)";

		constexpr float quadVertices[] = { // Vertex attributes for a quad that fills the entire screen in NDC
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
		};
		
		ScreenFramebuffer::ScreenFramebuffer(int width, int height) : Framebuffer(width, height),
			quadScreenShader(quadScreenShaderVertexSource, quadScreenShaderFragSource)
		{
			// Configure screen quad
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		}

		ScreenFramebuffer::~ScreenFramebuffer()
		{
			glDeleteVertexArrays(1, &quadVAO);
			glDeleteBuffers(1, &quadVBO);
		}


		void ScreenFramebuffer::PrepareForSceneRender()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
			glEnable(GL_DEPTH_TEST);
			glViewport(0, 0, width, height);
		}


		void ScreenFramebuffer::RenderToScreen()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			const int screenHeight{ Window::GetMainWindow()->GetWindowHeight() };
			const int screenWidth{ Window::GetMainWindow()->GetWindowWidth() };
			glViewport(0, 0, screenWidth, screenHeight);


			quadScreenShader.Use();
			glBindVertexArray(quadVAO);
			glDisable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);

		}
	}
}