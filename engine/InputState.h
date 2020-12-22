#pragma once

struct InputStateStruct
{
	bool showDebugInfo{ true };
	// volume, graphics settings etc etc to be added...
};

struct InputStateStructDebug
{
	char lastPressedButton;
	char lastReleasedButton;
};

class InputState
{
public:
	static InputStateStruct state;
	static InputStateStructDebug debugState;
};

