#include <iostream>
#include "Map.hpp"
#include "Car.hpp"
#include "CameraController.hpp"
#include "CoordinateMath.hpp"

Map::Map(double spawnLat, double spawnLon, int filter) : _centerLat(spawnLat), _centerLon(spawnLon), _chunkFilter(filter) {
	_mpt = coords::metersPerTile(spawnLat, Zoom);
	std::cout << "mpt: " << _mpt << '\n';

	int centerX = coords::lonToTileX(spawnLon, Zoom);
	int centerY = coords::latToTileY(spawnLat, Zoom);

	_centerTileX = centerX;
	_centerTileY = centerY;

	double tileLon = coords::tileXToLon(centerX, Zoom);
	double nextLon = coords::tileXToLon(centerX + 1, Zoom);
	double tileLat = coords::tileYToLat(centerY, Zoom);
	double nextLat = coords::tileYToLat(centerY + 1, Zoom);
	float fracX = (float)((spawnLon - tileLon) / (nextLon - tileLon));
	float fracZ = (float)((spawnLat - tileLat) / (nextLat - tileLat));
	float offsetX = (fracX - 0.5f) * (float)_mpt;
	float offsetZ = (fracZ - 0.5f) * (float)_mpt;
	_initialCarSpawn = { offsetX, 0.75f, offsetZ };

	for (int gy = -Radius; gy <= Radius; gy++) {
		for (int gx = -Radius; gx <= Radius; gx++) {
			int tileX = centerX + gx;
			int tileY = centerY + gy;
			int idx = (gy + Radius) * GridSize + (gx + Radius);
			_chunks[idx] = std::make_unique<MapChunk>(MapChunkCoords{ gx, gy }, tileX, tileY, _mpt);
		}
	}
}

void Map::render(Shader* overrideShader) {
	for (const auto& chunk : _chunks) {
		chunk->render(overrideShader);
	}
}

void Map::update(Car& car, CameraController& camera) {
	for (const auto& chunk : _chunks)
		chunk->uploadIfReady(_chunkFilter);

	const float half = (float)(_mpt * 0.5);
	Vector3 pos = car.pos();

	int dx = 0, dy = 0;

	if (pos.x > half) dx = 1;
	else if (pos.x < -half) dx = -1;

	if (pos.z > half) dy = 1;
	else if (pos.z < -half) dy = -1;

	if (dx == 0 && dy == 0) return;

	shiftGrid(dx, dy, car, camera);
}

Vector3 Map::getCarSpawn() const {
	return _initialCarSpawn;
}

double Map::centerLat() const {
	return _centerLat;
}

double Map::centerLon() const {
	return _centerLon;
}

void Map::shiftGrid(int dx, int dy, Car& car, CameraController& camera) {
	_centerTileX += dx;
	_centerTileY += dy;
	_centerLat = coords::tileYToLat(_centerTileY, Zoom);
	_centerLon = coords::tileXToLon(_centerTileX, Zoom);

	car.snapToOrigin((float)_mpt * dx, (float)_mpt * dy);
	camera.snapToOrigin((float)_mpt * dx, (float)_mpt * dy);

	for (const auto& chunk : _chunks) {
		chunk->updateGridCoord(_centerTileX, _centerTileY);
	}
}

double Map::mpt() const {
	return _mpt;
}

void Map::setShader(Shader shader) {
	for (auto& chunk : _chunks)
		chunk->setShader(shader);
}
