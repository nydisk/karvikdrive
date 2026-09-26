#include "Car.hpp"
#include "Map.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <string>
#include <raylib.h>
#include <raymath.h>
#include "Configuration.hpp"

Car::Car(Camera3D& cam, const char* modelPath, CarConfig config, Vector3 spawn)
	: _cam(cam), _config(config), _vel{}, _pos(spawn)
{
	_model = LoadModel(modelPath);
	BoundingBox bb = GetModelBoundingBox(_model);
	_pos.y = (bb.max.y - bb.min.y) / 2.0f;
}

void Car::render(Shader* overrideShader) const {
	if (overrideShader) {
		Shader prev = _model.materials[0].shader;
		_model.materials[0].shader = *overrideShader;
		DrawModelEx(_model, _pos, { 0.f, 1.f, 0.f }, -_dir - 90.0f, { 1,1,1 }, WHITE);
		_model.materials[0].shader = prev;
	}
	else {
		DrawModelEx(_model, _pos, { 0.f, 1.f, 0.f }, -_dir - 90.0f, { 1,1,1 }, WHITE);
	}
}

void Car::update(Map& map) {
	const float dt = GetFrameTime();
	if (dt <= 0.f) return;

	float headingRad = _dir * DEG2RAD;
	Vector3 forward = { cosf(headingRad), 0.f, sinf(headingRad) };
	Vector3 right = { cosf(headingRad + PI / 2.0f), 0.f, sinf(headingRad + PI / 2.0f) };

	float forwardVel = Vector3DotProduct(_vel, forward);
	float lateralVel = Vector3DotProduct(_vel, right);
	float absForwardVel = std::abs(forwardVel);
	float absLateralVel = std::abs(lateralVel);

	float weightTransferFactor = _config.cgHeight / _config.wheelbase;

	bool accelerating = IsKeyDown(KEY_W);
	bool braking = IsKeyDown(KEY_S);
	bool handbrake = IsKeyDown(KEY_SPACE);
	float steerInput = (IsKeyDown(KEY_D) ? 1.0f : 0.0f) - (IsKeyDown(KEY_A) ? 1.0f : 0.0f);

	float engineForce = 0.0f;
	if (accelerating && !handbrake) {
		float maxPowerWatts = _config.horsepower * 745.7f;
		float accelWeightBias = (1.0f - _config.frontWeightBias) * 0.3f;
		float maxTractionForce = _config.mass * 9.81f * _config.tireFriction * (0.9f + accelWeightBias);
		float powerLimitedForce = maxPowerWatts / std::max(absForwardVel, 1.0f);
		engineForce = std::min(powerLimitedForce, maxTractionForce);
	}

	float drag = forwardVel * absForwardVel * _config.dragCoeff;
	float rollingRes = (forwardVel != 0.0f ? (forwardVel > 0.f ? 1.0f : -1.0f) : 0.0f) * _config.mass * 0.015f * 9.81f;

	float engineBraking = 0.0f;
	if (!accelerating && !braking && !handbrake && forwardVel > 0.5f) {
		float engineBrakingForce = _config.mass * 9.81f * _config.engineBraking * std::clamp(absForwardVel / 10.0f, 0.2f, 1.0f);
		engineBraking = -engineBrakingForce;
	}

	float brakeForceVal = 0.0f;
	if (braking) {
		if (forwardVel > 0.01f) {
			brakeForceVal = -_config.mass * 9.81f * _config.brakeForce;
		}
		else {
			float maxPowerWatts = _config.horsepower * 745.7f * 0.4f;
			float maxTractionForce = _config.mass * 9.81f * _config.tireFriction * 0.5f;
			brakeForceVal = -std::min(maxPowerWatts / std::max(absForwardVel, 1.0f), maxTractionForce);
		}
	}

	float tmpForwardVel = forwardVel;
	if (braking && absForwardVel < 0.5f && forwardVel > 0.f) tmpForwardVel = 0.f;

	float brakingIntensity = 0.0f;
	if (braking && absForwardVel > 2.0f)
		brakingIntensity = std::clamp(std::abs(brakeForceVal) / (_config.mass * 9.81f * _config.brakeForce), 0.0f, 1.0f);

	float handbrakeDrag = 0.0f;
	if (handbrake && absForwardVel > 0.1f) {
		float hbFriction = _config.mass * 9.81f * _config.tireFriction * 0.8f;
		handbrakeDrag = (forwardVel > 0.f ? -1.0f : 1.0f) * hbFriction;
	}

	float accelZ = (engineForce + engineBraking + brakeForceVal + handbrakeDrag - drag - rollingRes) / _config.mass;

	float steerMax = _config.steerMaxAngle * DEG2RAD;
	float steerAngle = steerInput * steerMax;

	float speedFactor = absForwardVel / _config.steerSpeedRef;
	float gripFactor = 1.0f / (1.0f + speedFactor * speedFactor * _config.steerSpeedSharpness);

	float targetYaw = (forwardVel / _config.wheelbase) * tanf(steerAngle) * gripFactor;

	float slideAmount = std::clamp((absLateralVel - 3.0f) / 6.0f, 0.0f, 1.0f);
	float dynamicMaxYaw = Lerp(90.0f * DEG2RAD, 300.0f * DEG2RAD, slideAmount);
	targetYaw = std::clamp(targetYaw, -dynamicMaxYaw, dynamicMaxYaw);

	float yawLerp = absLateralVel > 6.0f ? dt * (_config.yawDamping * 0.4f) : dt * _config.yawDamping;
	_yawRate = Lerp(_yawRate, targetYaw, yawLerp);

	float brakingGripPenalty = brakingIntensity * _config.frontWeightBias * (1.0f + weightTransferFactor);
	float cgRollPenalty = std::clamp(absLateralVel * weightTransferFactor * 0.15f, 0.0f, 0.35f);

	float latGripScale = std::clamp(1.0f - absForwardVel * 0.005f, 0.5f, 1.0f);
	latGripScale *= (1.0f - brakingGripPenalty);
	latGripScale *= (1.0f - cgRollPenalty);
	if (handbrake) latGripScale *= _config.handbrakeGrip;

	float rearGripScale = latGripScale * _config.tireGripRear;
	float frontGripScale = latGripScale * _config.tireGripFront;
	float effectiveGripScale = Lerp(rearGripScale, frontGripScale, 0.35f);

	float maxGrip = _config.mass * 9.81f * _config.tireFriction;
	float latForce = std::clamp(-lateralVel * 22.0f * _config.mass * effectiveGripScale, -maxGrip, maxGrip);
	float accelX = latForce / _config.mass;

	forwardVel = tmpForwardVel + accelZ * dt;
	lateralVel = lateralVel + accelX * dt;

	if (handbrake && absForwardVel < 0.5f) forwardVel = 0.f;
	if (absForwardVel < 0.1f && !accelerating && !braking && !handbrake) forwardVel = 0.f;
	if (std::abs(lateralVel) < 0.05f) lateralVel = 0.f;

	_vel.x = forward.x * forwardVel + right.x * lateralVel;
	_vel.z = forward.z * forwardVel + right.z * lateralVel;

	float slideSuppress = std::clamp((absLateralVel - 4.0f) / 8.0f, 0.0f, 1.0f);
	float assistStrength = _config.stabilityAssist
		* (1.0f - brakingIntensity * 0.9f)
		* (1.0f - slideSuppress * 0.9f);
	if (handbrake) assistStrength = 0.0f;

	float velSpeed = Vector3Length(_vel);
	if (velSpeed > 0.5f) {
		float blendSpeed = dt * assistStrength * 3.0f;
		Vector3 targetVel = Vector3Scale(forward, forwardVel > 0.f ? velSpeed : -velSpeed);
		_vel.x = Lerp(_vel.x, targetVel.x, blendSpeed);
		_vel.z = Lerp(_vel.z, targetVel.z, blendSpeed);
	}

	_distanceTravelled += (double)(Vector3Length(_vel) * dt);
	_dir += _yawRate * dt * RAD2DEG;
	_pos.x += _vel.x * dt;
	_pos.z += _vel.z * dt;

	updateGeoPosition(map);
}

void Car::snapToOrigin(float dx, float dz) {
	_pos.x -= dx; _pos.z -= dz;
}

float Car::dir() const { return _dir; }
Vector3 Car::pos() const { return _pos; }
Vector3 Car::velocity() const { return _vel; }
double Car::lat() const { return _lat; }
double Car::lon() const { return _lon; }

double Car::distanceTravelled() const{
	return _distanceTravelled;
}

void Car::setShader(Shader shader) {
	_model.materials[0].shader = shader;
}

void Car::updateGeoPosition(Map& map) {
	constexpr double metersPerDegLat = 111320.0;
	const double metersPerDegLon = 111320.0 * std::cos(map.centerLat() * DEG2RAD);
	_lat = map.centerLat() + _pos.z / metersPerDegLat;
	_lon = map.centerLon() + _pos.x / metersPerDegLon;
}