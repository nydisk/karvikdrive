#pragma once
#include "UI_Base.hpp"

class Map;
class Car;
class CameraController;
class UI_Tools : public UI_Base {
	Map& _map;
	Car& _car;
	CameraController& _camera;

	float _teleportLat = 0.0f;
	float _teleportLon = 0.0f;
public:
	UI_Tools(Map& map, Car& car, CameraController& camera);

	void imguiDraw() override;
};