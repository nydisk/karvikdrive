#pragma once
#include <raylib.h>
#include "CarInfo.hpp"

class Map;

class Car {
	CarInfo _info;
	float _yawRate = 0.0f;
	float _dir = 0.0f;
	Vector3 _vel{};
	Vector3 _pos{};
	Model _model{};
	double _lat = 0.0;
	double _lon = 0.0;
	double _distanceTravelled = 0.0;
	Camera3D& _cam;
public:
	Car(Camera3D& cam, CarInfo config, Vector3 spawn);
	void render(Shader* overrideShader) const;
	void update(Map& map);
	void snapToOrigin(float dx, float dz);

	float dir() const;
	Vector3 pos() const;
	Vector3 velocity() const;
	double lat() const;
	double lon() const;
	double distanceTravelled() const;
	const CarInfo& info() const;

	void setShader(Shader shader);
private:
	void updateGeoPosition(Map& map);
};