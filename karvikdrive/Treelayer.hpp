#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <array>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <random>
#include <fstream>
#include <filesystem>
#include "CoordinateMath.hpp"
#include "Map.hpp"
#include "HttpClient.hpp"

#undef min
#undef max

namespace fs = std::filesystem;

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

struct TreeTile {
	int tx, ty; // tile pos
	int gx, gy; // 3x3 gridpos

	std::vector<Vector3> treePositions; // specific
	std::vector<Vector3> woodPositions; // scatter

	std::atomic<bool> pendingUpdate{ false };
	std::mutex mutex;

	TileTreeData pendingData;
};

class TreeLayer {
public:
	inline static constexpr int TreeZoom = 14;
	inline static constexpr int GridRadius = 1; // 3x3
	inline static constexpr int GridSize = GridRadius * 2 + 1;
	inline static constexpr int TileCount = GridSize * GridSize;
	inline static constexpr int WoodScatterDensity = 1; // trees per 100m^2
	inline static const std::string CacheDirectory = "cache/trees/";
private:
	double _centerLat, _centerLon;
	int _centerTileX, _centerTileY;

	std::array<std::shared_ptr<TreeTile>, TileCount> _tiles;
	std::vector<Vector3> _allTrees;
	std::vector<Vector3> _allWoods;
public:
	TreeLayer(double cLat, double cLon) : _centerLat(cLat), _centerLon(cLon) {
		_centerTileX = coords::lonToTileX(cLon, TreeZoom);
		_centerTileY = coords::latToTileY(cLat, TreeZoom);

		for (int gy = -GridRadius; gy <= GridRadius; gy++)
			for (int gx = -GridRadius; gx <= GridRadius; gx++)
				fetchTile(gx, gy);
	}

	void update(double cLat, double cLon, double oLat, double oLon) {
		uploadReady(oLat, oLon);
		shiftIfNeeded(cLat, cLon, oLat, oLon);
	}

	const std::vector<Vector3>& trees() const { return _allTrees; }
	const std::vector<Vector3>& woods() const { return _allWoods; }
private:
	int tileIdx(int gx, int gy) const {
		return (gy + GridRadius) * GridSize + (gx + GridRadius);
	}

	void fetchTile(int gx, int gy) {
		int tx = _centerTileX + gx;
		int ty = _centerTileY + gy;
		int idx = tileIdx(gx, gy);

		_tiles[idx] = std::make_shared<TreeTile>();
		_tiles[idx]->tx = tx;
		_tiles[idx]->ty = ty;
		_tiles[idx]->gx = gx;
		_tiles[idx]->gy = gy;

		std::shared_ptr<TreeTile> tilePtr = _tiles[idx]; // capture shared ownership

		std::thread([this, tilePtr, tx, ty]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			auto data = loadOrFetch(tx, ty);
			std::lock_guard lock(tilePtr->mutex);
			tilePtr->pendingData = std::move(data);
			tilePtr->pendingUpdate = true;
		}).detach();
	}

	void uploadReady(double oLat, double oLon) {
		bool changed = false;

		for (auto& tile : _tiles) {
			if (!tile || !tile->pendingUpdate) continue;
			std::lock_guard lock(tile->mutex);

			tile->treePositions.clear();
			tile->woodPositions.clear();

			for (auto& t : tile->pendingData.trees)
				tile->treePositions.push_back(latLonToWorld(t.lat, t.lon, oLat, oLon));

			for (auto& wood : tile->pendingData.woods)
				scatterInPolygon(wood, oLat, oLon, tile->woodPositions);

			tile->pendingUpdate = false;
			changed = true;
		}

		if (changed) rebuildAllPositions();
	}

	void shiftIfNeeded(double cLat, double cLon, double oLat, double oLon) {
		int ntx = coords::lonToTileX(cLon, TreeZoom);
		int nty = coords::latToTileY(cLat, TreeZoom);

		if (ntx == _centerTileX && nty == _centerTileY) return;

		_centerTileX = ntx;
		_centerTileY = nty;
		_centerLat = cLat;
		_centerLon = cLon;

		for (int gy = -GridRadius; gy <= GridRadius; gy++)
			for (int gx = -GridRadius; gx <= GridRadius; gx++)
				fetchTile(gx, gy);

		rebuildAllPositions();
	}

	void rebuildAllPositions() {
		_allTrees.clear();
		_allWoods.clear();
		for (auto& tile : _tiles) {
			if (!tile) continue;
			_allTrees.insert(_allTrees.end(), tile->treePositions.begin(), tile->treePositions.end());
			_allWoods.insert(_allWoods.end(), tile->woodPositions.begin(), tile->woodPositions.end());
		}
	}

	static Vector3 latLonToWorld(double lat, double lon, double originLat, double originLon) {
		constexpr double metersPerDegLat = 111320.0;
		double metersPerDegLon = 111320.0 * std::cos(originLat * DEG2RAD);
		return {
			(float)((lon - originLon) * metersPerDegLon),
			0.0f,
			(float)((lat - originLat) * metersPerDegLat)
		};
	}

	void scatterInPolygon(const WoodPolygon& poly, double oLat, double oLon, std::vector<Vector3>& out) const {
		if (poly.points.size() < 3) return;

		double minLat = 1e9, maxLat = -1e9;
		double minLon = 1e9, maxLon = -1e9;
		for (auto& [lat, lon] : poly.points) {
			minLat = std::min(minLat, lat); 
			maxLat = std::max(maxLat, lat);
			minLon = std::min(minLon, lon); 
			maxLon = std::max(maxLon, lon);
		}

		constexpr double mpdLat = 111320.0;
		double mpdLon = 111320.0 * std::cos(oLat * DEG2RAD);
		double widthM = (maxLon - minLon) * mpdLon;
		double heightM = (maxLat - minLat) * mpdLat;
		int count = (int)(widthM * heightM / 10000.0 * WoodScatterDensity);
		count = std::clamp(count, 0, 2000);

		std::mt19937 rng((unsigned int)(poly.points.size() * 2654435761u)); // deterministic per polygon
		std::uniform_real_distribution<double> latDist(minLat, maxLat);
		std::uniform_real_distribution<double> lonDist(minLon, maxLon);
		for (int i = 0; i < count * 4 && (int)out.size() < count; i++) {
			double lat = latDist(rng);
			double lon = lonDist(rng);
			if (pointInPolygon(lat, lon, poly.points))
				out.push_back(latLonToWorld(lat, lon, oLat, oLon));
		}
	}

	static bool pointInPolygon(double lat, double lon, const std::vector<std::pair<double, double>>& poly) {
		bool inside = false;
		int n = (int)poly.size();
		for (int i = 0, j = n - 1; i < n; j = i++) {
			double yi = poly[i].first, xi = poly[i].second;
			double yj = poly[j].first, xj = poly[j].second;
			if (((yi > lat) != (yj > lat)) &&
				(lon < (xj - xi) * (lat - yi) / (yj - yi) + xi))
				inside = !inside;
		}
		return inside;
	}

	static TileTreeData loadOrFetch(int tx, int ty) {
		fs::create_directories(CacheDirectory);
		std::string path = std::string(CacheDirectory) + std::to_string(tx) + "_" + std::to_string(ty) + ".json";

		if (fs::exists(path)) {
			std::ifstream f(path);
			std::string raw((std::istreambuf_iterator<char>(f)), {});
			if (raw.find("\"elements\"") != std::string::npos) // valid overpass response
				return parseResponse(raw);
			// else fall through and re-fetch
			std::cout << "treelayer: cached file invalid, re-fetching " << tx << "_" << ty << "\n";
		}

		double minLat = coords::tileYToLat(ty + 1, TreeZoom);
		double maxLat = coords::tileYToLat(ty, TreeZoom);
		double minLon = coords::tileXToLon(tx, TreeZoom);
		double maxLon = coords::tileXToLon(tx + 1, TreeZoom);

		std::string query = buildQuery(minLat, minLon, maxLat, maxLon);
		std::string raw = httpPost(query);

		if (raw.find("\"elements\"") != std::string::npos) {
			std::ofstream f(path);
			f << raw; // only cache if valid
		}
		else {
			std::cout << "treelayer: rate limited or bad response for " << tx << "_" << ty << "\n";
		}
		return parseResponse(raw);
	}

	static std::string buildQuery(double minLat, double minLon, double maxLat, double maxLon) {
		std::ostringstream ss;
		ss << std::fixed << std::setprecision(7);
		ss << "[out:json][timeout:25];("
			<< "node[\"natural\"=\"tree\"]("
			<< minLat << "," << minLon << "," << maxLat << "," << maxLon << ");"
			<< "way[\"natural\"=\"wood\"]("
			<< minLat << "," << minLon << "," << maxLat << "," << maxLon << ");"
			<< "way[\"landuse\"=\"forest\"]("
			<< minLat << "," << minLon << "," << maxLat << "," << maxLon << ");"
			<< ");out body geom;";
		return ss.str();
	}

	static std::string httpPost(const std::string& query) {
		std::string body = "data=" + urlEncode(query);
		return HttpClient::post(L"overpass-api.de", L"/api/interpreter", body);
	}

	static TileTreeData parseResponse(const std::string& json) {
		TileTreeData result;
		auto j = nlohmann::json::parse(json, nullptr, false);
		if (j.is_discarded()) return result;

		for (auto& el : j["elements"]) {
			std::string type = el.value("type", "");
			auto tags = el.value("tags", nlohmann::json::object());

			if (type == "node" && tags.value("natural", "") == "tree") {
				result.trees.push_back({ el["lat"], el["lon"] });
			}
			else if (type == "way") {
				WoodPolygon poly;
				for (auto& pt : el["geometry"])
					poly.points.push_back({ pt["lat"], pt["lon"] });
				result.woods.push_back(poly);
			}
		}
		return result;
	}

	static std::string urlEncode(const std::string& s) {
		std::ostringstream enc;
		for (unsigned char c : s) {
			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
				enc << c;
			else
				enc << '%' << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)c;
		}
		return enc.str();
	}
};