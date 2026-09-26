#pragma once
#include <raylib.h>

struct CarConfig {
	float mass;               // kg
	float horsepower;         // hp
	float wheelbase;          // meters, front-to-rear axle distance
	float dragCoeff;          // aerodynamic drag, tune for top speed
	float cgHeight;           // meters, center of gravity height
	float frontWeightBias;    // 0.0-1.0, fraction of weight on front axle
	float tireFriction;       // overall grip multiplier
	float tireGripFront;      // front tire grip relative to tireFriction
	float tireGripRear;       // rear tire grip relative to tireFriction
	float steerMaxAngle;      // degrees, maximum wheel lock angle
	float steerSpeedRef;      // m/s reference speed for understeer rolloff
	float steerSpeedSharpness;// how aggressively understeer rolls off
	float brakeForce;         // g-force of braking
	float handbrakeGrip;      // fraction of rear grip kept during handbrake
	float stabilityAssist;    // 0.0-1.0, how strongly velocity snaps to heading
	float yawDamping;         // how quickly yaw rate tracks target
	float engineBraking;      // thingy

	static CarConfig Civic() {
		return { 1300.f, 143.f, 2.7f, 0.5f, 0.55f, 0.62f, 1.0f, 1.0f, 0.95f, 34.f, 16.f, 1.5f, 0.9f, 0.15f, 0.4f, 4.0f, 0.04f };
	}
	static CarConfig SportsCar() {
		return { 1450.f, 420.f, 2.6f, 0.4f, 0.42f, 0.48f, 1.3f, 1.1f, 1.0f, 28.f, 22.f, 1.8f, 1.1f, 0.1f, 0.25f, 5.0f, 0.08f };
	}
	static CarConfig Truck() {
		return { 3500.f, 300.f, 3.8f, 1.2f, 1.1f, 0.45f, 0.85f, 0.9f, 0.85f, 38.f, 10.f, 1.2f, 0.65f, 0.25f, 0.55f, 3.0f, 0.10f };
	}
	static CarConfig Bus() {
		return { 12000.f, 350.f, 5.8f, 2.5f, 1.6f, 0.52f, 0.8f, 0.85f, 0.8f, 42.f, 8.f, 1.0f, 0.55f, 0.3f, 0.7f, 2.5f, 0.01f };
	}
};

class Map;

class Car {
	CarConfig _config;
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
	Car(Camera3D& cam, const char* modelPath, CarConfig config, Vector3 spawn);
	void render(Shader* overrideShader) const;
	void update(Map& map);
	void snapToOrigin(float dx, float dz);

	float dir() const;
	Vector3 pos() const;
	Vector3 velocity() const;
	double lat() const;
	double lon() const;
	double distanceTravelled() const;

	void setShader(Shader shader);
private:
	void updateGeoPosition(Map& map);
};