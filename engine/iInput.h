#pragma once

class iInput
{
public:
	virtual ~iInput() {}

	virtual void SetInputEnabled(bool enabled) { input_enabled = enabled; }
	bool GetInputEnabled() { return input_enabled; }

protected:
	bool input_enabled;
};