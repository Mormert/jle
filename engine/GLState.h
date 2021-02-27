#pragma once

namespace jle
{
	// Contains global data corresponding to the OpenGL State
	struct GLState
	{
		static unsigned int globalActiveTexture;
	};

	static GLState glStaticState;
}
