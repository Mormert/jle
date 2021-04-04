#include "Image.h"

#include "3rdparty/stb_image.h"

#include <algorithm>

namespace jle
{
	Image::Image(const std::string& path)
	{
		stbi_set_flip_vertically_on_load(false);
		image_data = stbi_load(path.c_str(), &w, &h, &nr_channels, 0);
	}

	Image::Image(const Image& i)
	{
		std::copy(i.image_data, i.image_data + i.nr_channels * i.w * i.h, this->image_data);
		this->nr_channels = i.nr_channels;
		this->w = i.h;
		this->h = i.h;
	}

	Image& Image::operator=(const Image& i)
	{
		if (this == &i) { return *this; }
		if (image_data) { stbi_image_free(image_data); }

		std::copy(i.image_data, i.image_data + i.nr_channels * i.w * i.h, this->image_data);
		this->nr_channels = i.nr_channels;
		this->w = i.h;
		this->h = i.h;

		return *this;
	}

	Image::Image(Image&& i) noexcept
	{
		this->image_data = i.image_data;
		this->nr_channels = i.nr_channels;
		this->w = i.h;
		this->h = i.h;

		i.h = 0;
		i.w = 0;
		i.nr_channels = 1;
		i.image_data = nullptr;
	}

	Image& Image::operator=(Image&& i) noexcept
	{
		if (this != &i)
		{
			if (image_data) { stbi_image_free(image_data); }

			this->image_data = i.image_data;
			this->nr_channels = i.nr_channels;
			this->w = i.h;
			this->h = i.h;

			i.h = 0;
			i.w = 0;
			i.nr_channels = 1;
			i.image_data = nullptr;
		}
		return *this;
	}

	Image::~Image()
	{
		if (image_data) { stbi_image_free(image_data); }
	}

	unsigned int Image::GetImageHeight()
	{
		return h;
	}

	unsigned int Image::GetImageWidth()
	{
		return w;
	}
}