#pragma once
#include <memory>
#include <array>
#include <raylib.h>
#include "MapChunk.hpp"

class Car;
class CameraController;
class Map {
public:
	static constexpr int GridSize = 15;
	static constexpr int RenderArea = GridSize * GridSize;
	static constexpr int Radius = GridSize / 2;
	static constexpr int Zoom = 20;
private:
	double _centerLat, _centerLon;
	int _centerTileX, _centerTileY;

	double _mpt;
	
	std::array<std::unique_ptr<MapChunk>, RenderArea> _chunks{};
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
private:
	void shiftGrid(int dx, int dy, Car& car, CameraController& camera);
};