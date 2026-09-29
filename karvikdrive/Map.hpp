#pragma once
#include <memory>
#include <array>
#include <raylib.h>
#include "MapChunk.hpp"

class Car;
class CameraController;
class Map {
public:
	static constexpr int kGridSize = 15;
	static constexpr int kRenderArea = kGridSize * kGridSize;
	static constexpr int kRadius = kGridSize / 2;
	static constexpr int kZoom = 20;
private:
	double _centerLat, _centerLon;
	int _centerTileX, _centerTileY;

	double _mpt;

	Shader _shader;
	bool _hasShader = false;
	
	std::array<std::unique_ptr<MapChunk>, kRenderArea> _chunks{};
	int _chunkFilter;
	
	Vector3 _initialCarSpawn;
public:
	Map(double spawnLat, double spawnLon, int chunkFilter = TEXTURE_FILTER_ANISOTROPIC_8X);

	void render(Shader* overrideShader);
	void update(Car& car, CameraController& camera);

	Vector3 getCarSpawn() const;

	double centerLat() const;
	double centerLon() const;
	double mpt() const;

	void setShader(Shader shader);

	void rebuild(double lat, double lon);
	void teleport(double lat, double lon, Car& car, CameraController& camera);
private:
	void shiftGrid(int dx, int dy, Car& car, CameraController& camera);
};