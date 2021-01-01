#include "Camera2D.h"

#include <glm/gtc/matrix_transform.hpp>

namespace jle
{


	Camera2D::Camera2D(unsigned int viewportWidth, unsigned int viewportHeight,
		unsigned int windowWidth, unsigned int windowHeight) : Viewport::Viewport(viewportWidth, viewportHeight,
			windowWidth, windowHeight)
	{
		height_or_width = viewportHeight;
		screenFramebuffer = std::make_unique<gfx::ScreenFramebuffer>(viewportWidth, viewportHeight);
	}

	glm::mat4 Camera2D::GetCameraMat() const noexcept
	{
		glm::mat4 cam{ 1.0f };

		cam = glm::ortho(static_cast<float>(GetWorldPositionX()),
			static_cast<float>(GetWorldPositionX()) + GetViewportWidth(),
			static_cast<float>(GetWorldPositionY()) + GetViewportHeight(),
			static_cast<float>(GetWorldPositionY()), -1.0f, 1.0f);

		return cam;
	}

	void Camera2D::SetAspectDependance(AspectDependOn aspect, int dimension)
	{
		height_or_width = dimension;
		this->aspect = aspect;
	}

	void Camera2D::SetWindowDimensions(unsigned int width, unsigned int height)
	{
		Viewport::SetWindowDimensions(width, height);

		if (aspect == AspectDependOn::height)
		{
			SetViewportHeightKeepAspect(height_or_width);
		}
		else // depend on width
		{
			SetViewportWidthKeepAspect(height_or_width);
		}

		screenFramebuffer = std::make_unique<gfx::ScreenFramebuffer>(viewportWidth, viewportHeight);

	}

}

