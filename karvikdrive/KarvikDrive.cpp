#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

#include "Configuration.hpp"
#include "ShaderManager.hpp"
#include "ShadowRenderer.hpp"
#include "Renderer.hpp"
#include "CameraController.hpp"
#include "Map.hpp"
#include "Car.hpp"
#include "DayNight.hpp"
#include "TileDownloader.hpp"
#include "Speedometer.hpp"
#include "CarLoader.hpp"

#include "UI_Base.hpp"
#include "UI_GeneralInformation.hpp"
#include "UI_Tools.hpp"
#include "UI_VehicleLoader.hpp"

using Cfg = Configuration;

//TODO: move these somewhere that isn't the main.cpp file lol
#pragma region Shader uniform initializations

void initFog(ShaderManager& shaders) {
	float fogNear = Map::kGridSize * Cfg::fogCfg.nearMultiplier;
	float fogFar = Map::kGridSize * Cfg::fogCfg.farMultiplier;
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
#pragma endregion

int main() {
	Cfg::loadConfigValues();
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(Cfg::graphicsCfg.width, Cfg::graphicsCfg.height, "karvikdrive");
	if (IsWindowFullscreen()) { if (!Cfg::graphicsCfg.fullscreen) ToggleFullscreen(); }
	else { if (Cfg::graphicsCfg.fullscreen) ToggleFullscreen(); }

	ShaderManager shaders{};
	ShadowRenderer shadows(shaders);
	Renderer renderer(shaders, shadows, Cfg::graphicsCfg.renderWidth, Cfg::graphicsCfg.renderHeight, Cfg::skyCfg, Cfg::starCfg, Cfg::quantizeCfg);
	CameraController camera{};
	Speedometer speedometer{};

	Map map(59.29960644714724, 24.65917325632329, TEXTURE_FILTER_ANISOTROPIC_8X);

	CarLoader carLoader("veh/");
	Car car(camera.camera(), carLoader.getCarInfo("civic"), map.getCarSpawn());

	DayNight dayNight(Cfg::dayNightCfg, map.mpt());
	
	initShaderUniforms(shaders, renderer, map, car);

	rlImGuiSetup(true);

	bool raiseShaderReload = false;
	std::vector<std::unique_ptr<UI_Base>> imguiElements{};
	imguiElements.push_back(std::make_unique<UI_GeneralInformation>(car));
	imguiElements.push_back(std::make_unique<UI_Tools>(map, car, camera));
	imguiElements.push_back(std::make_unique<UI_VehicleLoader>(map, car, camera, carLoader, raiseShaderReload));

	DisableCursor();
	while (!WindowShouldClose()) {
		float dt = GetFrameTime();

		if (IsKeyPressed(KEY_F5) || raiseShaderReload) { // reload shader keybind
			raiseShaderReload = false;
			shaders.reload();
			initShaderUniforms(shaders, renderer, map, car);
		}

		if (IsKeyPressed(KEY_F1)) { // toggle mouse cursor keybind
			if (IsCursorHidden()) 
				EnableCursor();
			else 
				DisableCursor();
		}

		map.update(car, camera);
		car.update(map);
		camera.update(car.pos(), car.dir(), dt);
		dayNight.update(dt, map.mpt());

		shadows.beginPass(dayNight.lightSpaceMatrix());
		map.render(&shadows.shader());
		car.render(&shadows.shader());
		shadows.endPass();

		renderer.draw(camera.camera(), map, car, dayNight, dt);

		speedometer.draw(car.velocity());

		rlImGuiBegin();
		for (const auto& element : imguiElements) 
			element->imguiDraw();
		rlImGuiEnd();

		EndDrawing();
	}

	rlImGuiShutdown();
	CloseWindow();

	TileDownloader::cleanUpCache();
	Cfg::saveConfigValues();
}