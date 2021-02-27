#include "FullscreenRendering_OpenGL.h"

#include "3rdparty/glad/glad.h"

#include "GLStateMachine.h"

#include <string>

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



FullscreenRendering_OpenGL::FullscreenRendering_OpenGL()
	: quadScreenShader{ quadScreenShaderVertexSource, quadScreenShaderFragSource }
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

FullscreenRendering_OpenGL::~FullscreenRendering_OpenGL()
{
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
}

void FullscreenRendering_OpenGL::RenderFramebufferFullscreen(iFramebuffer& framebuffer, unsigned int screenWidth, unsigned int screenHeight)
{

	glViewport(0, 0, screenWidth, screenHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	quadScreenShader.Use();
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, (unsigned int)framebuffer.GetTexture());
	jle::gfx::glStateMachine.globalActiveTexture = (unsigned int)framebuffer.GetTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
