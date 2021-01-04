#pragma once

#include "Window.h"

#include <string>
#include <memory>

namespace jle
{
	// Pimpl idiom, class definition exsists in DebugRenderer.cpp
	class DebugRendererImpl;

	class DebugRenderer
	{
	public:

		~DebugRenderer();

		// no copy, no move
		DebugRenderer(const DebugRenderer& dr) = delete;
		DebugRenderer& operator=(const DebugRenderer& dr) = delete;
		DebugRenderer(DebugRenderer&& dr) = delete;
		DebugRenderer& operator=(DebugRenderer&& dr) = delete;

		template <typename T>
		void AddText(T text);
		void AddText(const std::string& text);
		void AddText(const char* text);

	private:
		friend class Engine;

		DebugRenderer(void* glfwNative);
		void Render();


		/// Pimpl idiom implementation
			// Gets a const version of the pImpl
			const DebugRendererImpl* Pimpl() const { return pImpl.get(); };
			// Gets the Window Pimpl class
			DebugRendererImpl* Pimpl() { return pImpl.get(); };

			// Smart pointer to the implementation (exsists in DebugRenderer.cpp)
			std::unique_ptr<DebugRendererImpl> pImpl;

	};
}