#pragma once
#include <string>
#include "CarConfig.hpp"
 
struct CarInfo {
	std::string name;
	std::string modelPath;
	CarConfig config;
};