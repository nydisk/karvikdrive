#include <imgui.h>
#include <raylib.h>
#include <raymath.h>

#include "UI_Tools.hpp"
#include "Map.hpp"
#include "CameraController.hpp"
#include "Car.hpp"

UI_Tools::UI_Tools(Map& map, Car& car, CameraController& camera) : _map(map), _car(car), _camera(camera) {}

void UI_Tools::imguiDraw() {
	ImGui::Begin("Tools");

	if (ImGui::CollapsingHeader("Teleport")) {
		ImGui::InputFloat("Latitude", &_teleportLat, 0.0f, 0.0f, "%f");
		ImGui::InputFloat("Longitude", &_teleportLon, 0.0f, 0.0f, "%f");
		if (ImGui::Button("Go")) 
			_map.teleport(_teleportLat, _teleportLon, _car, _camera);
	}

	ImGui::End();
}
