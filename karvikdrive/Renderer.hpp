#pragma once
#include <array>
#include <raylib.h>
#include <raymath.h>
#include "rlgl.h"
#include "ShaderManager.hpp"
#include "ShadowRenderer.hpp"
#include "DayNight.hpp"
#include "Map.hpp"
#include "Car.hpp"
#include "Configuration.hpp"

using Cfg = Configuration;

class Renderer {
	ShaderManager& _shaders;
	ShadowRenderer& _shadows;

	int _renderWidth, _renderHeight;
	RenderTexture2D _sceneTex{};

	SkyConfig _skyCfg;
	StarConfig _starCfg;
	QuantizeConfig _quantizeCfg;

	std::vector<Vector3> _starPositions;
	float _starRotation = 0.0f;
	float _nightFactor = 0.0f;
public:
	Renderer(ShaderManager& shaders, ShadowRenderer& shadows, int renderWidth, int renderHeight, SkyConfig skyCfg = {}, StarConfig starCfg = {}, QuantizeConfig quantizeCfg = {})
		: _shaders(shaders), _shadows(shadows), _renderWidth(renderWidth), _renderHeight(renderHeight), _skyCfg(skyCfg), _starCfg(starCfg), _quantizeCfg(quantizeCfg)
	{
		_starCfg.orbitSpeed = Cfg::starCfg.orbitSpeed / Cfg::dayNightCfg.dayDurationSeconds;
		_sceneTex = LoadRenderTexture(renderWidth, renderHeight);
		SetTextureFilter(_sceneTex.texture, TEXTURE_FILTER_POINT);
		initQuantizeShader();
		initSkyShader();
		initStars();
	}

	~Renderer() {
		UnloadRenderTexture(_sceneTex);
	}

	void draw(Camera3D& cam, Map& map, Car& car, DayNight& dayNight, float dt) {
		updateStars(dt, dayNight);
		beginScene(dayNight);
		drawSky(dayNight);
		drawWorld(cam, map, car, dayNight);
		endScene();
		drawQuantize(cam);
	}

	void reinitShaders() {
		initQuantizeShader();
		initSkyShader();
	}

	int renderWidth()  const { return _renderWidth; }
	int renderHeight() const { return _renderHeight; }
private:
	void initQuantizeShader() {
		Shader& quantize = _shaders.get(ShaderType::Quantize);
		float rw = (float)_renderWidth;
		float rh = (float)_renderHeight;
		SetShaderValue(quantize, _shaders.loc(ShaderType::Quantize, "colorDepth"), &_quantizeCfg.colorDepth, SHADER_UNIFORM_INT);
		SetShaderValue(quantize, _shaders.loc(ShaderType::Quantize, "ditherStrength"), &_quantizeCfg.ditherStrength, SHADER_UNIFORM_FLOAT);
		SetShaderValue(quantize, _shaders.loc(ShaderType::Quantize, "rw"), &rw, SHADER_UNIFORM_FLOAT);
		SetShaderValue(quantize, _shaders.loc(ShaderType::Quantize, "rh"), &rh, SHADER_UNIFORM_FLOAT);
	}

	void initSkyShader() {
		Shader& sky = _shaders.get(ShaderType::Sky);
		float w = (float)_renderWidth;
		float h = (float)_renderHeight;
		_shaders.set(ShaderType::Sky, "screenWidth", &w, SHADER_UNIFORM_FLOAT);
		_shaders.set(ShaderType::Sky, "screenHeight", &h, SHADER_UNIFORM_FLOAT);
		_shaders.set(ShaderType::Sky, "bandWidth", &_skyCfg.bandWidth, SHADER_UNIFORM_FLOAT);
		_shaders.set(ShaderType::Sky, "horizonScale", &_skyCfg.horizonScale, SHADER_UNIFORM_FLOAT);
		_shaders.set(ShaderType::Sky, "zenithScale", &_skyCfg.zenithScale, SHADER_UNIFORM_FLOAT);
	}

	void setHeadlightUniforms(const Car& car) {
		float headingRad = car.dir() * DEG2RAD;
		Vector3 forward = { cosf(headingRad), 0.f, sinf(headingRad) };
		Vector3 right = { -sinf(headingRad), 0.f, cosf(headingRad) };
		Vector3 pos = car.pos();

		Vector3 base = Vector3Add(pos, Vector3Add(Vector3Scale(forward, car.info().config.headlightForwardOffset), Vector3{ 0.f, car.info().config.headlightHeightOffset, 0.f }));
		Vector3 posL = Vector3Add(base, Vector3Scale(right, -car.info().config.headlightSideOffset));
		Vector3 posR = Vector3Add(base, Vector3Scale(right, car.info().config.headlightSideOffset));
		Vector3 dir = Vector3Normalize(Vector3Subtract(forward, Vector3{ 0.f, car.info().config.headlightDownwardTilt, 0.f }));

		Vector3 color = { 1.0f, 0.95f, 0.85f };
		float range = 80.0f;
		float innerCos = cosf(18.0f * DEG2RAD);
		float outerCos = cosf(40.0f * DEG2RAD);

		auto apply = [&](ShaderType t) {
			_shaders.set(t, "headlightPosL", &posL, SHADER_UNIFORM_VEC3);
			_shaders.set(t, "headlightPosR", &posR, SHADER_UNIFORM_VEC3);
			_shaders.set(t, "headlightDir", &dir, SHADER_UNIFORM_VEC3);
			_shaders.set(t, "headlightColor", &color, SHADER_UNIFORM_VEC3);
			_shaders.set(t, "headlightRange", &range, SHADER_UNIFORM_FLOAT);
			_shaders.set(t, "headlightInnerCos", &innerCos, SHADER_UNIFORM_FLOAT);
			_shaders.set(t, "headlightOuterCos", &outerCos, SHADER_UNIFORM_FLOAT);
		};

		apply(ShaderType::Terrain);
		apply(ShaderType::Car);
	}

	void initStars() {
		_starPositions.resize(_starCfg.count);
		srand((unsigned int)time(NULL));
		for (auto& s : _starPositions) {
			float theta = ((float)rand() / RAND_MAX) * 2.0f * PI;
			float phi = acosf(1.0f - 2.0f * ((float)rand() / RAND_MAX));
			float r = _starCfg.radius;
			s = { r * sinf(phi) * cosf(theta), r * cosf(phi), r * sinf(phi) * sinf(theta) };
		}
	}

	void updateStars(float dt, const DayNight& dayNight) {
		float nt = dayNight.timeNormalized();
		_starRotation += dt * 2.0f * PI * _starCfg.orbitSpeed;

		float ns = _starCfg.nightStart;
		float ne = _starCfg.nightEnd;
		float fd = _starCfg.fadeDuration;

		_nightFactor = 0.0f;
		if (nt >= ns + fd || nt <= ne - fd)
			_nightFactor = 1.0f;
		else if (nt >= ns)
			_nightFactor = (nt - ns) / fd;
		else if (nt <= ne)
			_nightFactor = (ne - nt) / fd;
	}

	void beginScene(const DayNight& dayNight) {
		BeginTextureMode(_sceneTex);
		ClearBackground(BLACK);
	}

	void drawSky(const DayNight& dayNight) {
		Shader& sky = _shaders.get(ShaderType::Sky);
		Vector3 horizon = dayNight.fogColor();
		Vector3 zenith = dayNight.zenithColor();
		Vector3 sunDir = dayNight.sunDirection();
		float timeN = dayNight.timeNormalized();

		BeginShaderMode(sky);
		SetShaderValue(sky, _shaders.loc(ShaderType::Sky, "horizonColor"), &horizon, SHADER_UNIFORM_VEC3);
		SetShaderValue(sky, _shaders.loc(ShaderType::Sky, "zenithColor"), &zenith, SHADER_UNIFORM_VEC3);
		SetShaderValue(sky, _shaders.loc(ShaderType::Sky, "sunDirection"), &sunDir, SHADER_UNIFORM_VEC3);
		SetShaderValue(sky, _shaders.loc(ShaderType::Sky, "timeNorm"), &timeN, SHADER_UNIFORM_FLOAT);
		DrawRectangle(0, 0, _renderWidth, _renderHeight, WHITE);
		EndShaderMode();
	}

	void drawStars(Camera3D& cam) {
		if (_nightFactor <= 0.01f) return;

		Matrix starRot = MatrixRotateY(_starRotation * 0.3f) * MatrixRotateZ(_starRotation);

		Vector3 right = Vector3Normalize(Vector3CrossProduct(cam.target - cam.position, cam.up));
		Vector3 up = cam.up;
		float sz = _starCfg.size;
		Color col = Fade(WHITE, _nightFactor);

		rlDisableDepthMask();
		rlBegin(RL_QUADS);
		for (const auto& s : _starPositions) {
			Vector3 rotated = Vector3Transform(s, starRot);
			Vector3 wp = cam.position + rotated;
			rlColor4ub(col.r, col.g, col.b, col.a);
			rlVertex3f(wp.x - right.x * sz - up.x * sz, wp.y - right.y * sz - up.y * sz, wp.z - right.z * sz - up.z * sz);
			rlVertex3f(wp.x + right.x * sz - up.x * sz, wp.y + right.y * sz - up.y * sz, wp.z + right.z * sz - up.z * sz);
			rlVertex3f(wp.x + right.x * sz + up.x * sz, wp.y + right.y * sz + up.y * sz, wp.z + right.z * sz + up.z * sz);
			rlVertex3f(wp.x - right.x * sz + up.x * sz, wp.y - right.y * sz + up.y * sz, wp.z - right.z * sz + up.z * sz);
		}
		rlEnd();
		rlEnableDepthMask();
	}

	void drawWorld(Camera3D& cam, Map& map, Car& car, DayNight& dayNight){
		dayNight.applyToShader(_shaders.get(ShaderType::Terrain));
		dayNight.applyToShader(_shaders.get(ShaderType::Car));
		_shadows.bindToShader(ShaderType::Terrain, _shaders);
		_shadows.bindToShader(ShaderType::Car, _shaders);

		Vector3 camPos = cam.position;
		_shaders.set(ShaderType::Terrain, "cameraPos", &camPos, SHADER_UNIFORM_VEC3);
		_shaders.set(ShaderType::Car, "cameraPos", &camPos, SHADER_UNIFORM_VEC3);
		_shaders.set(ShaderType::Car, "viewPos", &camPos, SHADER_UNIFORM_VEC3);
		setHeadlightUniforms(car);

		BeginMode3D(cam);
		drawStars(cam);
		map.render(nullptr);
		car.render(nullptr);
		EndMode3D();
	}

	void endScene() {
		EndTextureMode();
	}

	void drawQuantize(Camera3D& cam) {
		Shader& quantize = _shaders.get(ShaderType::Quantize);
		int w = GetScreenWidth();
		int h = GetScreenHeight();

		BeginDrawing();
		ClearBackground(BLACK);
		BeginShaderMode(quantize);
		SetShaderValue(quantize, _shaders.loc(ShaderType::Quantize, "colorDepth"), &_quantizeCfg.colorDepth, SHADER_UNIFORM_INT);
		SetShaderValue(quantize, _shaders.loc(ShaderType::Quantize, "ditherStrength"), &_quantizeCfg.ditherStrength, SHADER_UNIFORM_FLOAT);
		DrawTexturePro(_sceneTex.texture, { 0, 0, (float)_renderWidth, -(float)_renderHeight }, { 0, 0, (float)w, (float)h }, { 0, 0 }, 0.0f, WHITE);
		EndShaderMode();
	}


};