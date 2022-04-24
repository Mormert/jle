#pragma once

#include <fstream>

namespace jle
{
	namespace cfg
	{
		template <typename T>
		inline void LoadEngineConfig(const std::string& cfgName, T& cfgJson)
		{
			std::ifstream i(cfgName);
			if (i.good())
			{
				nlohmann::json j;
				i >> j;

				jle::from_json(j, cfgJson);
			}else
            {
                LOG_ERROR << "Could not load engine config: " << cfgName;
            }
		}

		inline void SaveEngineConfig(const std::string& cfgName, nlohmann::json& j)
		{
			std::ofstream i(cfgName);
			if (i.good())
			{
				i << j.dump(4);
			}else
            {
                LOG_ERROR << "Could not save engine config: " << cfgName;
            }
		}
	}

}