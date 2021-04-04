#pragma once

#include <string>

namespace jle
{
	class Image
	{
	public:
		Image(const std::string& path);
		
		Image(const Image& i);
		Image& operator= (const Image& i);
		Image(Image&& i) noexcept;
		Image& operator=(Image&& i) noexcept;

		~Image();

		unsigned int GetImageWidth();
		unsigned int GetImageHeight();

	private:
		unsigned char* image_data { nullptr };
		int w, h, nr_channels;
	};
}