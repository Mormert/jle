#pragma once

#include "no_copy_no_move.h"

#include "Image.h"

namespace jle
{
	class iTexture
	{
		NO_COPY_NO_MOVE(iTexture)
	public:
		virtual ~iTexture() {}
		iTexture() = default;

		virtual bool IsActive() = 0;
		virtual void SetToActiveTexture() = 0;

		virtual int GetWidth();
		virtual int GetHeight();
		virtual int GetNrChannels();

        virtual unsigned int GetTextureID() = 0;

	protected:
		int width = 0, height = 0, nr_channels = 0;
	};

}