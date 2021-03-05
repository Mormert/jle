#pragma once

#include "jleCore.h"

#include "iFullscreenRendering.h"

namespace jle
{
	class jleGameEngine : public jle::jleCore
	{
	public:
		virtual ~jleGameEngine() {}

		jleGameEngine(CoreSettings cs);

		// Main framebuffer
		std::shared_ptr<iFramebuffer> framebuffer_main;

	private:
		std::unique_ptr<iFullscreenRendering> fullscreen_renderer;

		virtual void Start(CoreSettings cs) override;

		void FramebufferResizeEvent(unsigned int width, unsigned int height);

	protected:
		virtual void Update(float dt) override;
		virtual void Render() override;
	};
}