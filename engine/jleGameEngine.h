#pragma once

#include "jleCore.h"

#include "jleGame.h"

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
		void SetGame(std::unique_ptr<jleGame> game);

		// Main framebuffer
		std::shared_ptr<iFramebuffer> framebuffer_main;

		
		void SetGameDimsPixels(FIXED_AXIS fa, unsigned int pixels);

	private:
		std::unique_ptr<iFullscreenRendering> fullscreen_renderer;

		void FramebufferResizeEvent(unsigned int width, unsigned int height);



	protected:
		virtual void Start() override;
		virtual void Update(float dt) override;
		virtual void Render() override;


		std::pair<unsigned int, unsigned int> GetFramebufferDimensions(FIXED_AXIS fa, unsigned int pixels_along_axis, unsigned int windowWidth, unsigned int windowHeight);

		FIXED_AXIS fixed_axis;

		// Game dimensions in pixels, along axis specified by fixed_axis.
		unsigned int gameDimsPixels;

		std::unique_ptr<jleGame> game;

	};
}