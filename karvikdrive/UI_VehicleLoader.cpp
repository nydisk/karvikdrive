#include <imgui.h>
#include <raylib.h>
#include <raymath.h>

#include "UI_VehicleLoader.hpp"
#include "Map.hpp"
#include "CameraController.hpp"
#include "Car.hpp"
#include "CarLoader.hpp"

UI_VehicleLoader::UI_VehicleLoader(Map& map, Car& car, CameraController& camera, CarLoader& loader, bool& raiseShaderReload) : _map(map), _car(car), _camera(camera), _loader(loader), _raiseShaderReload(raiseShaderReload) {

}

void UI_VehicleLoader::imguiDraw() {
	ImGui::Begin("Vehicle Loader");
	if (ImGui::BeginCombo("Vehicle", _car.info().name.c_str())) {
		for (const auto& [name, info] : _loader.getAllCars()) {
			bool isSelected = (_car.info().name == name);
			if (ImGui::Selectable(name.c_str(), isSelected)) {
				_car.switchVehicle(info);
				_raiseShaderReload = true;	
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::End();
}
