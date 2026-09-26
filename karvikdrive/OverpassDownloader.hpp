#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <winhttp.h>
#include "ext/json.hpp"
#include "CoordinateMath.hpp"

class OverpassDownloader {
	inline static std::string Endpoint = "overpass-api.de";
	inline static std::string Path = "/api/interpreter";
public:
	struct TreeNode {
		double lat, lon;
	};

	struct WoodPolygon {
		std::vector<std::pair<double, double>> points;
	};

	struct TileTreeData {
		std::vector<TreeNode> trees;
		std::vector<WoodPolygon> woods;
	};

	static TileTreeData fetchTile(int tx, int ty, int zoom) {
		double minLat = coords::tileYToLat(ty + 1, zoom);
		double maxLat = coords::tileYToLat(ty, zoom);
		double minLon = coords::tileXToLon(tx, zoom);
		double maxLon = coords::tileXToLon(tx + 1, zoom);

		std::string query = buildQuery(minLat, minLon, maxLat, maxLon);
		std::string response = httpPost(query);
		return parseResponse(response);
	}
private:
    
};