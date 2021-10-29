#pragma once

#include <fstream>

namespace jle
{
	namespace cfg
	{
		const std::string GameSettingsName = "jle_gs_config.json";
		const std::string EngineSettingsName = "jle_es_config.json";

		template <typename T>
		void LoadEngineConfig(const std::string& cfgName, T& cfgJson)
		{
			std::ifstream i(cfgName);
			if (i.good())
			{
				nlohmann::json j;
				i >> j;

				jle::from_json(j, cfgJson);
			}
		}

		void SaveEngineConfig(const std::string& cfgName, nlohmann::json& j)
		{
			std::ofstream i(cfgName);
			if (i.good())
			{
				i << j.dump(4);
			}
		}
	}

}