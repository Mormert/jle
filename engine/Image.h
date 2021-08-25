#pragma once

#include "FileLoadInterface.h"

#include <string>

namespace jle
{
	class Image : FileLoadInterface
	{
	public:
		Image() = default;
		Image(const std::string& path);
		
		virtual bool LoadFromFile(const std::string& path) override;

		Image(const Image& i);
		Image& operator= (const Image& i);
		Image(Image&& i) noexcept;
		Image& operator=(Image&& i) noexcept;

		~Image();

		unsigned int GetImageWidth() const;
		unsigned int GetImageHeight() const;

		unsigned int GetImageNrChannels() const;

		unsigned char* GetImageData() const;

	private:
		unsigned char* image_data { nullptr };
		int w = 0, h = 0, nr_channels = 0;
	};
}