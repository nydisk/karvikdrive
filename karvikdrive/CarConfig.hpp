#pragma once
#include "ext/json.hpp"
#include "JSONHelper.hpp"

using json = nlohmann::json;

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

	//static CarConfig Civic() {
	//	return { 1300.f, 143.f, 2.7f, 0.5f, 0.55f, 0.62f, 1.0f, 1.0f, 0.95f, 34.f, 16.f, 1.5f, 0.9f, 0.15f, 0.4f, 4.0f, 0.04f };
	//}
	//static CarConfig SportsCar() {
	//	return { 1450.f, 420.f, 2.6f, 0.4f, 0.42f, 0.48f, 1.3f, 1.1f, 1.0f, 28.f, 22.f, 1.8f, 1.1f, 0.1f, 0.25f, 5.0f, 0.08f };
	//}
	//static CarConfig Truck() {
	//	return { 3500.f, 300.f, 3.8f, 1.2f, 1.1f, 0.45f, 0.85f, 0.9f, 0.85f, 38.f, 10.f, 1.2f, 0.65f, 0.25f, 0.55f, 3.0f, 0.10f };
	//}
	//static CarConfig Bus() {
	//	return { 12000.f, 350.f, 5.8f, 2.5f, 1.6f, 0.52f, 0.8f, 0.85f, 0.8f, 42.f, 8.f, 1.0f, 0.55f, 0.3f, 0.7f, 2.5f, 0.01f };
	//}
};

inline void to_json(json& j, const CarConfig& c) {
	j = json{
		{"mass", c.mass},
		{"horsepower", c.horsepower},
		{"wheelbase", c.wheelbase},
		{"dragCoeff", c.dragCoeff},
		{"cgHeight", c.cgHeight},
		{"frontWeightBias", c.frontWeightBias},
		{"tireFriction", c.tireFriction},
		{"tireGripFront", c.tireGripFront},
		{"tireGripRear", c.tireGripRear},
		{"steerMaxAngle", c.steerMaxAngle},
		{"steerSpeedRef", c.steerSpeedRef},
		{"steerSpeedSharpness", c.steerSpeedSharpness},
		{"brakeForce", c.brakeForce},
		{"handbrakeGrip", c.handbrakeGrip},
		{"stabilityAssist", c.stabilityAssist},
		{"yawDamping", c.yawDamping},
		{"engineBraking", c.engineBraking},
	};
}

inline void from_json(const json& j, CarConfig& c) {
	c.mass = j.value("mass", c.mass);
	c.horsepower = j.value("horsepower", c.horsepower);
	c.wheelbase = j.value("wheelbase", c.wheelbase);
	c.dragCoeff = j.value("dragCoeff", c.dragCoeff);
	c.cgHeight = j.value("cgHeight", c.cgHeight);
	c.frontWeightBias = j.value("frontWeightBias", c.frontWeightBias);
	c.tireFriction = j.value("tireFriction", c.tireFriction);
	c.tireGripFront = j.value("tireGripFront", c.tireGripFront);
	c.tireGripRear = j.value("tireGripRear", c.tireGripRear);
	c.steerMaxAngle = j.value("steerMaxAngle", c.steerMaxAngle);
	c.steerSpeedRef = j.value("steerSpeedRef", c.steerSpeedRef);
	c.steerSpeedSharpness = j.value("steerSpeedSharpness", c.steerSpeedSharpness);
	c.brakeForce = j.value("brakeForce", c.brakeForce);
	c.handbrakeGrip = j.value("handbrakeGrip", c.handbrakeGrip);
	c.stabilityAssist = j.value("stabilityAssist", c.stabilityAssist);
	c.yawDamping = j.value("yawDamping", c.yawDamping);
	c.engineBraking = j.value("engineBraking", c.engineBraking);
}
