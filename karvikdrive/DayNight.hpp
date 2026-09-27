#pragma once
#include <raylib.h>
#include <raymath.h>
#include "WorldConfig.hpp"

// 0.0 = midnight, 0.25 = sunrise, 0.5 = noon, 0.75 = sunset
struct TimeOfDayColors {
    Vector3 sunColor;
    Vector3 ambientColor;
    Vector3 fogColor;
    Vector3 zenithColor;
    float sunIntensity;
};

class DayNight {
public:
    static constexpr int kShadowMapSize = 4096;

    explicit DayNight(DayNightConfig cfg, double mpt);

    void update(float dt, double mpt);

    void applyToShader(Shader& shader) const;

    float timeNormalized() const;
    Vector3 sunDirection() const;
    Matrix lightSpaceMatrix() const;
    Vector3 zenithColor() const;
    Vector3 fogColor() const;
private:
    DayNightConfig _cfg;
    float _timeNorm;

    Vector3 _sunDir;
    Matrix _lightSpaceMatrix;
    Vector3 _sunColor;
    Vector3 _ambientColor;
    Vector3 _fogColor;
    Vector3 _zenithColor;
    float _sunIntensity;

    void computeSunTransform(float gridWorldSize);
    static TimeOfDayColors sampleColors(float t);
};