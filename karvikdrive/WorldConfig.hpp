#pragma once
#include <algorithm>
#include "ext/json.hpp"
#include "JSONHelper.hpp"
using json = nlohmann::json;

#undef max
#undef min

struct DayNightConfig {
    float dayDurationSeconds = 120.0f;
    float startTimeNormalized = 0.25f;
    void fixValues() {
        dayDurationSeconds = std::max(dayDurationSeconds, 1.0f);
        startTimeNormalized = std::clamp(startTimeNormalized, 0.0f, 1.0f);
    }
};

inline void to_json(json& j, const DayNightConfig& c) {
    j = json{
        {"dayDurationSeconds",  c.dayDurationSeconds},
        {"startTimeNormalized", c.startTimeNormalized}
    };
}

inline void from_json(const json& j, DayNightConfig& c) {
    c.dayDurationSeconds = j.value("dayDurationSeconds", c.dayDurationSeconds);
    c.startTimeNormalized = j.value("startTimeNormalized", c.startTimeNormalized);
    c.fixValues();
}

struct SkyConfig {
	float bandWidth = 0.8f;
	float horizonScale = 1.6f;
	float zenithScale = 0.9f;
	void fixValues() {
		bandWidth = std::max(bandWidth, 0.0f);
		horizonScale = std::max(horizonScale, 0.0f);
		zenithScale = std::max(zenithScale, 0.0f);
	}
};

inline void to_json(json& j, const SkyConfig& g) {
	j = json{
		{"bandWidth", r(g.bandWidth)},
		{"horizonScale", r(g.horizonScale)},
		{"zenithScale", r(g.zenithScale)}
	};
}

inline void from_json(const json& j, SkyConfig& g) {
	g.bandWidth = j.value("bandWidth", g.bandWidth);
	g.horizonScale = j.value("horizonScale", g.horizonScale);
	g.zenithScale = j.value("zenithScale", g.zenithScale);
	g.fixValues();
}

struct StarConfig {
	int count = 1000;
	float radius = 500.0f;
	float size = 0.7f;
	float nightStart = 0.70f;
	float nightEnd = 0.30f;
	float fadeDuration = 0.08f;
	float orbitSpeed = 0.3f;

	void fixValues() {
		count = std::max(count, 0);
		radius = std::max(radius, 1.0f);
		size = std::max(size, 0.001f);
		nightStart = std::clamp(nightStart, 0.0f, 1.0f);
		nightEnd = std::clamp(nightEnd, 0.0f, 1.0f);
		fadeDuration = std::max(fadeDuration, 0.001f);
		orbitSpeed = std::max(orbitSpeed, 0.0f);
	}
};

inline void to_json(json& j, const StarConfig& c) {
	j = json{
		{"count", c.count},
		{"radius", r(c.radius)},
		{"size", r(c.size)},
		{"nightStart", r(c.nightStart)},
		{"nightEnd", r(c.nightEnd)},
		{"fadeDuration", r(c.fadeDuration)},
		{"orbitSpeed", r(c.orbitSpeed)}
	};
}

inline void from_json(const json& j, StarConfig& c) {
	c.count = j.value("count", c.count);
	c.radius = j.value("radius", c.radius);
	c.size = j.value("size", c.size);
	c.nightStart = j.value("nightStart", c.nightStart);
	c.nightEnd = j.value("nightEnd", c.nightEnd);
	c.fadeDuration = j.value("fadeDuration", c.fadeDuration);
	c.orbitSpeed = j.value("orbitSpeed", c.orbitSpeed);
	c.fixValues();
}

struct QuantizeConfig {
	int colorDepth = 16;
	float ditherStrength = 0.35f;
	float vertexSnapping = 1.1f;

	void fixValues() {
		colorDepth = std::max(colorDepth, 1);
		ditherStrength = std::max(ditherStrength, 0.0f);
		vertexSnapping = std::max(vertexSnapping, 0.0f);
	}
};

inline void to_json(json& j, const QuantizeConfig& c) {
	j = json{
		{"colorDepth", c.colorDepth},
		{"ditherStrength", r(c.ditherStrength)},
		{"vertexSnapping", r(c.vertexSnapping)},
	};
}

inline void from_json(const json& j, QuantizeConfig& c) {
	c.colorDepth = j.value("colorDepth", c.colorDepth);
	c.ditherStrength = j.value("ditherStrength", c.ditherStrength);
	c.vertexSnapping = j.value("vertexSnapping", c.vertexSnapping);
	c.fixValues();
}

struct FogConfig {
	float nearMultiplier = 2.0f;
	float farMultiplier = 12.0f;

	void fixValues() {
		nearMultiplier = std::max(0.0f, nearMultiplier);
		farMultiplier = std::max(nearMultiplier + 0.1f, farMultiplier);
	}
};

inline void to_json(json& j, const FogConfig& c) {
	j = json{
		{"nearMultiplier", r(c.nearMultiplier)},
		{"farMultiplier", r(c.farMultiplier)}
	};
}

inline void from_json(const json& j, FogConfig& c) {
	c.nearMultiplier = j.value("nearMultiplier", c.nearMultiplier);
	c.farMultiplier = j.value("farMultiplier", c.farMultiplier);
	c.fixValues();
}