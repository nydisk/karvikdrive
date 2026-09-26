#include <raylib.h>
#include "Configuration.hpp"
#include "ShaderManager.hpp"
#include "ShadowRenderer.hpp"
#include "Renderer.hpp"
#include "CameraController.hpp"
#include "Map.hpp"
#include "Car.hpp"
#include "DayNight.hpp"
#include "TileDownloader.hpp"
#include "HUD.hpp"
#include "Treelayer.hpp"

using Cfg = Configuration;

void initFog(ShaderManager& shaders) {
	float fogNear = Map::GridSize * Cfg::fogCfg.nearMultiplier;
	float fogFar = Map::GridSize * Cfg::fogCfg.farMultiplier;
	shaders.set(ShaderType::Terrain, "fogNear", &fogNear, SHADER_UNIFORM_FLOAT);
	shaders.set(ShaderType::Terrain, "fogFar", &fogFar, SHADER_UNIFORM_FLOAT);
	shaders.set(ShaderType::Car, "fogNear", &fogNear, SHADER_UNIFORM_FLOAT);
	shaders.set(ShaderType::Car, "fogFar", &fogFar, SHADER_UNIFORM_FLOAT);
}

void initSnap(ShaderManager& shaders) {
	shaders.set(ShaderType::Terrain, "snapStrength", &Cfg::quantizeCfg.vertexSnapping, SHADER_UNIFORM_FLOAT);
	shaders.set(ShaderType::Car, "snapStrength", &Cfg::quantizeCfg.vertexSnapping, SHADER_UNIFORM_FLOAT);
}

void initShaderUniforms(ShaderManager& shaders, Renderer& renderer, Map& map, Car& car) {
	map.setShader(shaders.get(ShaderType::Terrain));
	car.setShader(shaders.get(ShaderType::Car));
	initFog(shaders);
	initSnap(shaders);
	renderer.reinitShaders();
}

int main() {
	Cfg::loadConfigValues();
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(Cfg::graphicsCfg.width, Cfg::graphicsCfg.height, "ballsack");
	if (IsWindowFullscreen()) { if (!Cfg::graphicsCfg.fullscreen) ToggleFullscreen(); }
	else { if (Cfg::graphicsCfg.fullscreen) ToggleFullscreen(); }

	ShaderManager shaders{};
	ShadowRenderer shadows(shaders);
	Renderer renderer(shaders, shadows, Cfg::graphicsCfg.renderWidth, Cfg::graphicsCfg.renderHeight, Cfg::skyCfg, Cfg::starCfg, Cfg::quantizeCfg);
	TreeRenderer treeRenderer(shaders);
	CameraController camera{};
	HUD hud{};

	Map map(59.29960644714724, 24.65917325632329, TEXTURE_FILTER_ANISOTROPIC_8X);
	TreeLayer treeLayer(map.centerLat(), map.centerLon());
	Car car(camera.camera(), "mdl/civic.obj", CarConfig::Civic(), map.getCarSpawn());

	DayNight dayNight(Cfg::dayNightCfg, map.mpt());
	
	initShaderUniforms(shaders, renderer, map, car);

	DisableCursor();

	while (!WindowShouldClose()) {
		float dt = GetFrameTime();

		if (IsKeyPressed(KEY_F5)) { // reload shader keybind
			shaders.reload();
			initShaderUniforms(shaders, renderer, map, car);
		}

		map.update(car, camera);
		treeLayer.update(car.lat(), car.lon(), map.centerLat(), map.centerLon());
		car.update(map);
		camera.update(car.pos(), car.dir(), dt);
		dayNight.update(dt, map.mpt());

		shadows.beginPass(dayNight.lightSpaceMatrix());
		map.render(&shadows.shader());
		car.render(&shadows.shader());
		treeRenderer.render(treeLayer.trees(), treeLayer.woods(), &shadows.shader());
		shadows.endPass();

		renderer.draw(camera.camera(), map, car, dayNight, treeRenderer, treeLayer.trees(), treeLayer.woods(), dt);

		hud.draw(car.velocity(), car.lat(), car.lon(), car.distanceTravelled());
		EndDrawing();
	}

	TileDownloader::cleanUpCache();
	Cfg::saveConfigValues();
}