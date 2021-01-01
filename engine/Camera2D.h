#pragma once

#include "Viewport.h"
#include "ScreenFramebuffer.h"

#include <memory>

namespace jle
{
	class Camera2D : public Viewport
	{

	public:

		enum class AspectDependOn { height, width } aspect{ AspectDependOn::height }; // default to height

		Camera2D(unsigned int viewportWidth, unsigned int viewportHeight,
			unsigned int windowWidth, unsigned int windowHeight);

		// Returns a 4x4 matrix representing the 2D camera for OpenGL
		glm::mat4 GetCameraMat() const noexcept;

		void SetWindowDimensions(unsigned int width, unsigned int height);

		void SetAspectDependance(AspectDependOn aspect, int dimension);		

	private:

		// can act as height or width, depending on what aspect is set to
		int height_or_width;

		friend class Renderer2D;
		std::unique_ptr<gfx::ScreenFramebuffer> screenFramebuffer;

	};
}

