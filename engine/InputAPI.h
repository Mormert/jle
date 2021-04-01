#pragma once

#include "iInput.h"

#include "iKeyboardInput.h"
#include "iMouseInput.h"

#include <memory>

namespace jle
{
	class InputAPI : public iInput
	{
	public:
		virtual ~InputAPI() {}

		InputAPI(std::shared_ptr<iKeyboardInput> ki, std::shared_ptr<iMouseInput> mi) : keyboard{ ki }, mouse{ mi } {}

		std::shared_ptr<iKeyboardInput> keyboard;
		std::shared_ptr<iMouseInput> mouse;

		// Set to false if the input system should stop polling
		virtual void SetInputEnabled(bool enabled) override
		{
			iInput::SetInputEnabled(enabled);

			keyboard->SetInputEnabled(enabled);
			mouse->SetInputEnabled(enabled);
		}

		/*// Gets the X mouse position
		virtual int GetMouseX() noexcept;
		// Gets the X mouse position
		virtual int GetMouseY() noexcept;

		// Gets the X position in the world where the mouse hovers over
		virtual int GetMouseWorldX() noexcept;
		// Gets the Y position in the world where the mouse hovers over
		virtual int GetMouseWorldY() noexcept;*/


	};
}