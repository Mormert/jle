#pragma once

struct Camera2d
{
	/*enum class AspectDependance{ width, height } aspect;

	// Camera Zoom in pixels, along the axis set by aspect variable
	// where the other axis is depending on the aspect ratio for the window
	float cameraZoom = 100;*/

	float zoom = 1.f;

	float xPosition = 0.f, yPosition = 0.f;

	float rotation = 0.f;
};