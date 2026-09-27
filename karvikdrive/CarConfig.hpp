#pragma once
#include "ext/json.hpp"
#include "JSONHelper.hpp"

using json = nlohmann::json;

namespace {
	constexpr float kRadToDeg = 57.29577951308232f;
	constexpr float kDegToRad = 0.017453292519943295f;
}

struct CarConfig {
	float mass;                   // kg
	float horsepower;             // hp
							      
	float wheelbase;              // meters, front-to-rear axle distance
							      
	float dragCoeff;              // aerodynamic drag, tune for top speed
							      
	float cgHeight;               // meters, center of gravity height
							      
	float frontWeightBias;        // 0.0-1.0, fraction of weight on front axle
							      
	float tireFriction;           // overall grip multiplier
	float tireGripFront;          // front tire grip relative to tireFriction
	float tireGripRear;           // rear tire grip relative to tireFriction
							      
	float steerMaxAngle;          // degrees, maximum wheel lock angle
	float steerSpeedRef;          // m/s reference speed for understeer rolloff
	float steerSpeedSharpness;    // how aggressively understeer rolls off
	
	float brakeForce;             // g-force of braking
	float handbrakeGrip;          // fraction of rear grip kept during handbrake
	
	float stabilityAssist;        // 0.0-1.0, how strongly velocity snaps to heading
	
	float yawDamping;             // how quickly yaw rate tracks target
	
	float engineBraking;          // thingy

	float headlightForwardOffset; // meters, origin to front bumper (where the beams originate from yk)
	float headlightSideOffset;    // meters, half the distance between the two headlights
	float headlightHeightOffset;  // meters, headlight height above car origin
	float headlightDownwardTilt;  // how much the beam angles down toward the road

	float headlightColorR;        // 0.0-1.0, headlight tint red
	float headlightColorG;        // 0.0-1.0, headlight tint green
	float headlightColorB;        // 0.0-1.0, headlight tint blue

	float headlightRange;         // meters, distance before the beam fades to nothing
	float headlightInnerCos;      // cos(angle), full brightness within this cone
	float headlightOuterCos;      // cos(angle), beam fades to zero past this cone

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
	json engine = {
		{"horsepower", c.horsepower},
		{"engineBraking", c.engineBraking},
	};
	json chassis = {
		{"mass", c.mass},
		{"wheelbase", c.wheelbase},
		{"cgHeight", c.cgHeight},
		{"frontWeightBias", c.frontWeightBias},
	};
	json aerodynamics = {
		{"dragCoeff", c.dragCoeff},
	};
	json tires = {
		{"tireFriction", c.tireFriction},
		{"tireGripFront", c.tireGripFront},
		{"tireGripRear", c.tireGripRear},
	};
	json steering = {
		{"steerMaxAngle", c.steerMaxAngle},
		{"steerSpeedRef", c.steerSpeedRef},
		{"steerSpeedSharpness", c.steerSpeedSharpness},
	};
	json brakes = {
		{"brakeForce", c.brakeForce},
		{"handbrakeGrip", c.handbrakeGrip},
	};
	json stability = {
		{"stabilityAssist", c.stabilityAssist},
		{"yawDamping", c.yawDamping},
	};
	json headlights = {
		{"forwardOffset", c.headlightForwardOffset},
		{"sideOffset", c.headlightSideOffset},
		{"heightOffset", c.headlightHeightOffset},
		{"downwardTilt", c.headlightDownwardTilt},
		{"colorR", c.headlightColorR},
		{"colorG", c.headlightColorG},
		{"colorB", c.headlightColorB},
		{"range", c.headlightRange},
		{"innerAngle", acosf(c.headlightInnerCos) * kRadToDeg},
		{"outerAngle", acosf(c.headlightOuterCos) * kRadToDeg},
	};

	j = json{
		{"engine", engine},
		{"chassis", chassis},
		{"aerodynamics", aerodynamics},
		{"tires", tires},
		{"steering", steering},
		{"brakes", brakes},
		{"stability", stability},
		{"headlights", headlights},
	};
}

inline void from_json(const json& j, CarConfig& c) {
	auto sec = [&](const char* name) -> json {
		return j.contains(name) ? j.at(name) : json::object();
	};

	json engine = sec("engine");
	c.horsepower = engine.value("horsepower", c.horsepower);
	c.engineBraking = engine.value("engineBraking", c.engineBraking);

	json chassis = sec("chassis");
	c.mass = chassis.value("mass", c.mass);
	c.wheelbase = chassis.value("wheelbase", c.wheelbase);
	c.cgHeight = chassis.value("cgHeight", c.cgHeight);
	c.frontWeightBias = chassis.value("frontWeightBias", c.frontWeightBias);

	json aerodynamics = sec("aerodynamics");
	c.dragCoeff = aerodynamics.value("dragCoeff", c.dragCoeff);

	json tires = sec("tires");
	c.tireFriction = tires.value("tireFriction", c.tireFriction);
	c.tireGripFront = tires.value("tireGripFront", c.tireGripFront);
	c.tireGripRear = tires.value("tireGripRear", c.tireGripRear);

	json steering = sec("steering");
	c.steerMaxAngle = steering.value("steerMaxAngle", c.steerMaxAngle);
	c.steerSpeedRef = steering.value("steerSpeedRef", c.steerSpeedRef);
	c.steerSpeedSharpness = steering.value("steerSpeedSharpness", c.steerSpeedSharpness);

	json brakes = sec("brakes");
	c.brakeForce = brakes.value("brakeForce", c.brakeForce);
	c.handbrakeGrip = brakes.value("handbrakeGrip", c.handbrakeGrip);

	json stability = sec("stability");
	c.stabilityAssist = stability.value("stabilityAssist", c.stabilityAssist);
	c.yawDamping = stability.value("yawDamping", c.yawDamping);

	json headlights = sec("headlights");
	c.headlightForwardOffset = headlights.value("forwardOffset", c.headlightForwardOffset);
	c.headlightSideOffset = headlights.value("sideOffset", c.headlightSideOffset);
	c.headlightHeightOffset = headlights.value("heightOffset", c.headlightHeightOffset);
	c.headlightDownwardTilt = headlights.value("downwardTilt", c.headlightDownwardTilt);
	c.headlightColorR = headlights.value("colorR", c.headlightColorR);
	c.headlightColorG = headlights.value("colorG", c.headlightColorG);
	c.headlightColorB = headlights.value("colorB", c.headlightColorB);
	c.headlightRange = headlights.value("range", c.headlightRange);

	float innerAngleDeg = headlights.value("innerAngleDeg", acosf(c.headlightInnerCos) * kRadToDeg);
	float outerAngleDeg = headlights.value("outerAngleDeg", acosf(c.headlightOuterCos) * kRadToDeg);
	c.headlightInnerCos = cosf(innerAngleDeg * kDegToRad);
	c.headlightOuterCos = cosf(outerAngleDeg * kDegToRad);
}
