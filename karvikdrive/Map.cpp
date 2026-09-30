#include <iostream>
#include "Map.hpp"
#include "Car.hpp"
#include "CameraController.hpp"
#include "CoordinateMath.hpp"

Map::Map(double spawnLat, double spawnLon, int filter) : _centerLat(spawnLat), _centerLon(spawnLon), _chunkFilter(filter) {
	rebuild(spawnLat, spawnLon);
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

void Map::rebuild(double lat, double lon) {
	std::cout << "Map: rebuilding map at lat: " << lat << ", lon: " << lon << std::endl;

	_centerLat = lat;
	_centerLon = lon;
	_mpt = coords::metersPerTile(lat, kZoom);

	int centerX = coords::lonToTileX(lon, kZoom);
	int centerY = coords::latToTileY(lat, kZoom);
	_centerTileX = centerX;
	_centerTileY = centerY;

	double tileLon = coords::tileXToLon(centerX, kZoom);
	double nextLon = coords::tileXToLon(centerX + 1, kZoom);
	double tileLat = coords::tileYToLat(centerY, kZoom);
	double nextLat = coords::tileYToLat(centerY + 1, kZoom);
	float fracX = (float)((lon - tileLon) / (nextLon - tileLon));
	float fracZ = (float)((lat - tileLat) / (nextLat - tileLat));
	_initialCarSpawn = { (fracX - 0.5f) * (float)_mpt, 0.75f, (fracZ - 0.5f) * (float)_mpt };

	for (int gy = -kRadius; gy <= kRadius; gy++) {
		for (int gx = -kRadius; gx <= kRadius; gx++) {
			int idx = (gy + kRadius) * kGridSize + (gx + kRadius);
			_chunks[idx] = std::make_unique<MapChunk>(MapChunkCoords{ gx, gy }, centerX + gx, centerY + gy, _mpt);
			if (_hasShader) _chunks[idx]->setShader(_shader);
		}
	}
}

void Map::teleport(double lat, double lon, Car& car, CameraController& camera) {
	Vector3 oldPos = car.pos();
	std::cout << "Map: teleporting to lat: " << lat << ", lon: " << lon << std::endl;
	rebuild(lat, lon);
	Vector3 target = _initialCarSpawn;

	float dx = oldPos.x - target.x;
	float dz = oldPos.z - target.z;
	car.snapToOrigin(dx, dz);
	camera.snapToOrigin(dx, dz);
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
	_centerLat = coords::tileYToLat(_centerTileY, kZoom);
	_centerLon = coords::tileXToLon(_centerTileX, kZoom);

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
	_shader = shader;
	_hasShader = true;
	for (auto& chunk : _chunks)
		chunk->setShader(shader);
}
