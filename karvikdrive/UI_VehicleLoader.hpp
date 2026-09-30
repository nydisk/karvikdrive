#pragma once
#include "UI_Base.hpp"

class Map;
class Car;
class CameraController;
class CarLoader;
class UI_VehicleLoader : public UI_Base {
	Map& _map;
	Car& _car;
	CameraController& _camera;
	CarLoader& _loader;
	bool& _raiseShaderReload;
public:
	UI_VehicleLoader(Map& map, Car& car, CameraController& camera, CarLoader& loader, bool& raiseShaderReload);

	void imguiDraw() override;
};