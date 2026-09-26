#include "DayNight.hpp"
#include "WorldConfig.hpp"
#include "Map.hpp"
#include <cmath>
#include <algorithm>
#include <raymath.h>
#include <rlgl.h>

static constexpr TimeOfDayColors kAnchors[4] = {
    // midnight
    { {0.05f,0.05f,0.15f}, {0.03f,0.03f,0.08f}, {0.02f,0.02f,0.08f}, {0.01f,0.01f,0.05f}, 0.0f  },
    // sunrise
    { {1.0f,0.6f,0.3f},    {0.4f,0.3f,0.25f},   {0.7f,0.5f,0.4f},   {0.2f,0.3f,0.6f},    0.6f  },
    // noon
    { {1.0f,0.98f,0.9f},   {0.5f,0.52f,0.55f},  {0.6f,0.72f,0.85f}, {0.1f,0.3f,0.7f},    1.0f  },
    // sunset
    { {1.0f,0.5f,0.2f},    {0.35f,0.25f,0.2f},  {0.65f,0.45f,0.35f},{0.15f,0.2f,0.5f},   0.55f },
};

DayNight::DayNight(DayNightConfig cfg, double mpt)
    : _cfg(cfg), _timeNorm(cfg.startTimeNormalized)
{
    update(0.0f, mpt);
}

void DayNight::update(float dt, double mpt) {
    _timeNorm += dt / _cfg.dayDurationSeconds;
    if (_timeNorm >= 1.0f) _timeNorm -= 1.0f;

    float sunAngle = (_timeNorm - 0.25f) * 2.0f * PI;
    float elevationRad = sinf(sunAngle) * (PI / 2.0f);
    float azimuthRad = sunAngle;

    _sunDir = {
        -cosf(elevationRad) * sinf(azimuthRad),
        -sinf(elevationRad),
        -cosf(elevationRad) * cosf(azimuthRad)
    };
    _sunDir = Vector3Normalize(_sunDir);

    auto c = sampleColors(_timeNorm);
    _sunColor = c.sunColor;
    _ambientColor = c.ambientColor;
    _fogColor = c.fogColor;
    _sunIntensity = c.sunIntensity;
    _zenithColor = c.zenithColor;

    const float gridWorldSize = Map::GridSize * (float)mpt;
    computeSunTransform(gridWorldSize);
}

void DayNight::computeSunTransform(float gridWorldSize) {
    float half = gridWorldSize * 0.1f;

    Vector3 lightPos = Vector3Scale(Vector3Negate(_sunDir), half * 2.0f);
    Matrix view = MatrixLookAt(lightPos, Vector3Zero(), Vector3UnitY);
    Matrix proj = MatrixOrtho(-half, half, -half, half, 0.1f, half * 6.0f);

    _lightSpaceMatrix = MatrixMultiply(view, proj);
}

void DayNight::applyToShader(Shader& shader) const {
    int locLSM = GetShaderLocation(shader, "lightSpaceMatrix");
    SetShaderValueMatrix(shader, locLSM, _lightSpaceMatrix);

    int locDir = GetShaderLocation(shader, "sunDirection");
    SetShaderValue(shader, locDir, &_sunDir, SHADER_UNIFORM_VEC3);

    Vector3 sunColorScaled = { _sunColor.x * _sunIntensity, _sunColor.y * _sunIntensity, _sunColor.z * _sunIntensity };
    int locSun = GetShaderLocation(shader, "sunColor");
    SetShaderValue(shader, locSun, &sunColorScaled, SHADER_UNIFORM_VEC3);

    int locAmb = GetShaderLocation(shader, "ambientColor");
    SetShaderValue(shader, locAmb, &_ambientColor, SHADER_UNIFORM_VEC3);

    int locFog = GetShaderLocation(shader, "fogColor");
    SetShaderValue(shader, locFog, &_fogColor, SHADER_UNIFORM_VEC3);

    int locShadowMap = GetShaderLocation(shader, "shadowMap");
}

float DayNight::timeNormalized() const {
    return _timeNorm;
}

Vector3 DayNight::sunDirection() const {
    return _sunDir; 
}

Matrix DayNight::lightSpaceMatrix() const {
    return _lightSpaceMatrix; 
}

Vector3 DayNight::zenithColor() const {
    return _zenithColor;
}

Vector3 DayNight::fogColor() const {
    return _fogColor;
}

TimeOfDayColors DayNight::sampleColors(float t) {
    float scaled = t * 4.0f;
    int i0 = (int)scaled % 4;
    int i1 = (i0 + 1) % 4;
    float frac = scaled - (int)scaled;

    const auto& a = kAnchors[i0];
    const auto& b = kAnchors[i1];
    float s = frac * frac * (3.0f - 2.0f * frac);

    return {
        Vector3Lerp(a.sunColor, b.sunColor, s),
        Vector3Lerp(a.ambientColor, b.ambientColor, s),
        Vector3Lerp(a.fogColor, b.fogColor, s),
        Vector3Lerp(a.zenithColor, b.zenithColor, s),
        a.sunIntensity + (b.sunIntensity - a.sunIntensity) * s
    };
}