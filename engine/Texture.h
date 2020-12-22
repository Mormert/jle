#pragma once

#include <string>
#include "3rdparty/stb_image.h"

class Texture
{
public:
	Texture(std::string texturePath);
	virtual ~Texture();


	Texture(const Texture& t) = delete;
	Texture& operator= (const Texture& t) = delete;
	Texture(Texture&& t) = delete;
	Texture& operator=(Texture&& t) = delete;


	bool IsActive();
	void SetToActiveTexture();
	
	int GetWidth();
	int GetHeight();

private:
	
	const std::string texturePath;
	bool isCreated = false;

	int width = 0, height = 0, nrChannels = 0;
	unsigned int texture_id = UINT_MAX; // OpenGL Texture ID

	static unsigned int globalActiveTexture;

};