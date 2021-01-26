#pragma once

#include <string>

class iWindowAPI
{
public:
	virtual ~iWindowAPI() {};

	virtual void SetWindowSettings(unsigned int width, unsigned int height, const std::string &title) = 0;

	virtual void SetFpsMode(bool enable) = 0;
	virtual bool IsFpsMode() = 0;

	virtual unsigned int GetWindowHeight() = 0;
	virtual unsigned int GetWindowWidth() = 0;

};