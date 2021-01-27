#pragma once

#include "iKeyboardInput.h"
#include "iWindowLinkable.h"
#include "iWindowInternalAPI.h"

class KeyboardInputInternal : public iKeyboardInput, public iWindowLinkable
{
public:
	KeyboardInputInternal(std::shared_ptr<iWindowInternalAPI> windowInternal);
	
	void LinkWindow(std::shared_ptr<iWindowInternalAPI> windowInternal) override;

	bool GetKeyPressed(char key) override;

	bool GetKeyReleased(char key) override;

	bool GetKeyDown(char key) override;

private:
	std::shared_ptr<iWindowInternalAPI> windowInternal;
};