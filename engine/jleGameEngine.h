#pragma once

#include "jleCore.h"

#include "iFullscreenRendering.h"

namespace jle
{
	// Specifies which axis to be fixed, and the other scales depending on window aspect ratio
	enum class FIXED_AXIS { same_as_window, width, height };

	struct GameSettings : public CoreSettings
	{
		struct 
		{
			FIXED_AXIS fixedAxis{ FIXED_AXIS::same_as_window };
			unsigned int fixedAxisPixels{ 0 };
		} framebufferSettings;

		virtual ~GameSettings() {}
	};

	class jleGameEngine : public jle::jleCore
	{
	public:
		virtual ~jleGameEngine() {}

		jleGameEngine(std::shared_ptr<GameSettings> gs);

		// Main framebuffer
		std::shared_ptr<iFramebuffer> framebuffer_main;

		void SetGameDimsPixels(FIXED_AXIS fa, unsigned int pixels);

	private:
		std::unique_ptr<iFullscreenRendering> fullscreen_renderer;

		virtual void Start() override;

		void FramebufferResizeEvent(unsigned int width, unsigned int height);

		FIXED_AXIS fixed_axis;

		// Game dimensions in pixels, along axis specified by fixed_axis.
		unsigned int gameDimsPixels;

	protected:
		virtual void Update(float dt) override;
		virtual void Render() override;
	};
}