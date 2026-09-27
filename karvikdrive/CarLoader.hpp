#pragma once
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <format>
#include <filesystem>
#include "ext/json.hpp"
#include "CarInfo.hpp"

using json = nlohmann::json;

class CarLoader {
	std::unordered_map<std::string, CarInfo> _cars{};
public:
	CarLoader(const std::string& discoveryFolder) {
		discoverCars(discoveryFolder);
	}

	const CarInfo& getCarInfo(const std::string& name) const {
		auto it = _cars.find(name);
		if (it == _cars.end()) {
			throw std::runtime_error("CarLoader: car not found '" + name + "'");
		}
		return it->second;
	}
private:
	void discoverCars(const std::string& folder) {
		for (const auto& i : std::filesystem::directory_iterator(folder)) {
			if (!i.is_directory()) continue;

			std::string carName = i.path().stem().string();
			std::string carFolder = i.path().string();
			std::string configPath = std::format("{}/handling.json", carFolder);
			std::string modelPath = std::format("{}/{}.obj", carFolder, carName);

			if (std::filesystem::exists(configPath) && std::filesystem::exists(modelPath)) {
				CarInfo info;

				info.handling = LoadCarConfig(configPath);
				info.name = carName;
				info.modelPath = modelPath;
				_cars[info.name] = info;
			}
			else {
				std::cerr << "CarLoader: missing config or model for car: " << carName << '\n';
			}
		}

		std::cout << "CarLoader: discovered " << _cars.size() << " cars\n";
	}

	CarConfig LoadCarConfig(const std::string& path) {
		std::ifstream f(path);
		if (!f.is_open()) {
			throw std::runtime_error("Failed to open car config file: " + path);
		}

		try {
			json j;
			f >> j;

			return j.get<CarConfig>();
		}
		catch (const json::parse_error& e) {
			std::cerr << "JSON parse error in: " << path << '\n';
			std::cerr << e.what() << '\n';
			throw;
		}
	}
};