#pragma once
#include <algorithm>
#include "ext/json.hpp"
#include "JSONHelper.hpp"
using json = nlohmann::json;

#undef max
#undef min

struct CacheConfig {
	uint64_t maximumCacheSizeMB = 400;
	double cacheKeepPercentage = 0.8f;

	void fixValues() {
		maximumCacheSizeMB = std::max((uint64_t)10, maximumCacheSizeMB);
		cacheKeepPercentage = std::clamp(cacheKeepPercentage, 0.01, 0.99);
	}
};

inline void to_json(json& j, const CacheConfig& c) {
	j = json{
		{"maximumCacheSizeMB", c.maximumCacheSizeMB},
		{"cacheKeepPercentage", rd(c.cacheKeepPercentage)}
	};
}

inline void from_json(const json& j, CacheConfig& c) {
	c.maximumCacheSizeMB = j.value("maximumCacheSizeMB", c.maximumCacheSizeMB);
	c.cacheKeepPercentage = j.value("cacheKeepPercentage", c.cacheKeepPercentage);
	c.fixValues();
}

struct GraphicsConfig {
	unsigned int width = 1920;
	unsigned int height = 1080;
	int renderWidth = 640;
	int renderHeight = 360;
	float uiScale = 1.5f;
	bool fullscreen = false;

	void fixValues() {
		width = std::max(10u, width);
		height = std::max(10u, height);
		renderWidth = std::max(1, renderWidth);
		renderHeight = std::max(1, renderHeight);
		uiScale = std::max(0.0f, uiScale);
	}
};

inline void to_json(json& j, const GraphicsConfig& c) {
	j = json{
		{"width", c.width},
		{"height", c.height},
		{"renderWidth", c.renderWidth},
		{"renderHeight", c.renderHeight},
		{"uiScale", r(c.uiScale)},
		{"fullscreen", c.fullscreen}
	};
}

inline void from_json(const json& j, GraphicsConfig& c) {
	c.width = j.value("width", c.width);
	c.height = j.value("height", c.height);
	c.renderWidth = j.value("renderWidth", c.renderWidth);
	c.renderHeight = j.value("renderHeight", c.renderHeight);
	c.uiScale = j.value("uiScale", c.uiScale);
	c.fullscreen = j.value("fullscreen", c.fullscreen);
	c.fixValues();
}