#include <imgui.h>
#include <raylib.h>
#include <raymath.h>

#include "UI_GeneralInformation.hpp"
#include "Car.hpp"

UI_GeneralInformation::UI_GeneralInformation(const Car& car) : _car(car) {}

void UI_GeneralInformation::imguiDraw() {
	ImGui::Begin("Details");

	float speedKmh = Vector3Length(_car.velocity()) * 3.6f;
	ImGui::Text("Speed: %.2f km/h", speedKmh);

	double distanceTravelledKm = _car.distanceTravelled() / 1000.0;
	ImGui::Text("Travelled: %.2f km", distanceTravelledKm);

	double lat = _car.lat();
	double lon = _car.lon();
	ImGui::Text("Latitude: %.6f", lat);
	ImGui::Text("Longitude: %.6f", lon);

	// ImGui::SeparatorText("Day/Night cycle");

	ImGui::End();
}
