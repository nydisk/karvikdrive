#pragma once
#include <string>
#include <algorithm>
#include <fstream>
#include "ext/json.hpp"
#include "WorldConfig.hpp"
#include "SystemConfig.hpp"

#undef max

using json = nlohmann::json;

class Configuration {
	inline static std::string ConfigFile = "config.json";
public:
	inline static CacheConfig cacheCfg;
	inline static GraphicsConfig graphicsCfg;
	inline static FogConfig fogCfg;
	inline static SkyConfig skyCfg;
	inline static StarConfig starCfg;
	inline static QuantizeConfig quantizeCfg;
	inline static DayNightConfig dayNightCfg;

	inline static void loadConfigValues() {
		std::ifstream file(ConfigFile);
		if (!file.is_open()) {
			saveConfigValues();
			return;
		}

		json j{};
		file >> j;

		file.close();

		if (j.contains("cache")) cacheCfg = j.at("cache").get<CacheConfig>();
		if (j.contains("graphics")) graphicsCfg = j.at("graphics").get<GraphicsConfig>();

		if (j.contains("fog")) fogCfg = j.at("fog").get<FogConfig>();
		if (j.contains("sky")) skyCfg = j.at("sky").get<SkyConfig>();
		if (j.contains("stars")) starCfg = j.at("stars").get<StarConfig>();
		if (j.contains("quantize")) quantizeCfg = j.at("quantize").get<QuantizeConfig>();
		if (j.contains("dayNight")) dayNightCfg = j.at("dayNight").get<DayNightConfig>();
	}

	inline static void saveConfigValues() {
		json j;
		j["fog"] = fogCfg;
		j["sky"] = skyCfg;
		j["stars"] = starCfg;
		j["quantize"] = quantizeCfg;
		j["dayNight"] = dayNightCfg;

		j["cache"] = cacheCfg;
		j["graphics"] = graphicsCfg;

		std::ofstream file(ConfigFile);
		file << j.dump(4);
		file.close();
	}
};