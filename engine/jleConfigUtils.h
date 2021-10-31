#pragma once

#include <fstream>

namespace jle
{
	namespace cfg
	{
		inline const std::string GameSettingsName = "jle_gs_config.json";
		inline const std::string EngineSettingsName = "jle_es_config.json";

		template <typename T>
		inline void LoadEngineConfig(const std::string& cfgName, T& cfgJson)
		{
			std::ifstream i(cfgName);
			if (i.good())
			{
				nlohmann::json j;
				i >> j;

				jle::from_json(j, cfgJson);
			}
		}

		inline void SaveEngineConfig(const std::string& cfgName, nlohmann::json& j)
		{
			std::ofstream i(cfgName);
			if (i.good())
			{
				i << j.dump(4);
			}
		}
	}

}