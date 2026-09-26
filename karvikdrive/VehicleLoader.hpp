#pragma once
#include "ext/json.hpp"
#include <filesystem>
namespace fs = std::filesystem;
using json = nlohmann::json;

class VehicleLoader {
	inline static const std::string VehicleDataDirectory = "veh/";
public:
	void discoverVehicles() {

	}
private:

};