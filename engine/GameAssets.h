#pragma once

#include <string>

namespace jle
{
	class GameAssets
	{

	public:
		static void SetGameAssetsPath(const std::string& path);

		//static Image GetImage(char* image_name);


	private:
		static std::string gameAssetsPath;

	};

}