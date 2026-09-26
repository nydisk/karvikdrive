#pragma once
#include <cmath>

namespace coords {
	static double tileXToLon(int x, int z) {
		return x / std::pow(2.0, z) * 360.0 - 180.0;
	}

	static double tileYToLat(int y, int z) {
		double n = PI - 2.0 * PI * y / std::pow(2.0, z);
		return 180.0 / PI * std::atan(0.5 * (std::exp(n) - std::exp(-n)));
	}

	static int lonToTileX(double lon, int z) {
		return (int)std::floor((lon + 180.0) / 360.0 * std::pow(2.0, z));
	}

	static int latToTileY(double lat, int z) {
		return (int)std::floor((1.0 - std::log(std::tan(lat * PI / 180.0) + 1.0 / std::cos(lat * PI / 180.0)) / PI) / 2.0 * std::pow(2.0, z));
	}

	static double metersPerTile(double lat, int z) {
		return 40075016.686 * std::cos(lat * PI / 180.0) / std::pow(2.0, z);
	}
}