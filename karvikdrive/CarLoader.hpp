#pragma once
#include <string>
#include "CarInfo.hpp"

class CarLoader {
	std::unordered_map<std::string, CarInfo> _cars{};
public:
	CarLoader(const std::string& discoveryFolder);

	const CarInfo& getCarInfo(const std::string& name) const;
	const std::unordered_map<std::string, CarInfo>& getAllCars() const;
private:
	void discoverCars(const std::string& folder);

	CarConfig LoadCarConfig(const std::string& path);
};