#pragma once

namespace jle
{
	namespace gfx
	{
		// Contains global data corresponding to the GL State Machine on the GPU
		struct GLStateMachine
		{
			static unsigned int globalActiveTexture;
		};

		static GLStateMachine glStateMachine;
	}
}
