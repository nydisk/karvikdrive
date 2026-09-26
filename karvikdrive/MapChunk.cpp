#include <iostream>
#include "MapChunk.hpp"
#include "TileDownloader.hpp"
#include "CoordinateMath.hpp"
#include "Map.hpp"
#include "rlgl.h"


MapChunk::MapChunk(MapChunkCoords coords, int tileX, int tileY, double& mpt)
	: _coords(coords), _tileX(tileX), _tileY(tileY), _mpt(mpt) {
	_mesh = GenMeshPlane((float)_mpt, (float)_mpt, 1, 1);
	_model = LoadModelFromMesh(_mesh);
	updateTexture();
}

void MapChunk::render(Shader* overrideShader) {
	if (overrideShader) {
		Shader prev = _model.materials[0].shader;
		_model.materials[0].shader = *overrideShader;
		DrawModel(_model, getPosition(), 1.0f, WHITE);
		_model.materials[0].shader = prev;
	}
	else {
		DrawModel(_model, getPosition(), 1.0f, WHITE);
	}
}

void MapChunk::renderDebug() {
	DrawModelWires(_model, getPosition(), 1.0f, RED);
}

void MapChunk::updateGridCoord(int centerTileX, int centerTileY) {
	int newX = _tileX - centerTileX;
	int newY = _tileY - centerTileY;

	if (std::abs(newX) > Map::Radius || std::abs(newY) > Map::Radius) {
		if (newX > Map::Radius) newX -= Map::GridSize;
		else if (newX < -Map::Radius) newX += Map::GridSize;
		if (newY > Map::Radius) newY -= Map::GridSize;
		else if (newY < -Map::Radius) newY += Map::GridSize;

		_coords = { newX, newY };
		_tileX = centerTileX + newX;
		_tileY = centerTileY + newY;
		updateTexture();
	}
	else {
		_coords = { newX, newY };
	}
}

MapChunkCoords MapChunk::coords() const {
	return _coords;
}

void MapChunk::setShader(Shader shader) {
	_model.materials[0].shader = shader;
}

Vector3 MapChunk::getPosition() {
	return {
		(float)_coords.x * (float)_mpt,
		0.0f,
		(float)_coords.y * (float)_mpt
	};
}

void MapChunk::updateTexture() {
	std::thread([this]() {
		auto bytes = TileDownloader::fetchTile(_tileX, _tileY, Map::Zoom);
		std::lock_guard lock(_mutex);
		_pendingBytes = std::move(bytes);
		_pendingUpdate = true;
	}).detach();
}


void MapChunk::uploadIfReady(int filter) {
	if (!_pendingUpdate) return;
	std::lock_guard lock(_mutex);
	if (_pendingBytes.empty()) return;

	if (_currentTexture.id > 0) UnloadTexture(_currentTexture);
	Image temp = LoadImageFromMemory(".png", _pendingBytes.data(), (int)_pendingBytes.size());
	_currentTexture = LoadTextureFromImage(temp);
	UnloadImage(temp);

	SetTextureFilter(_currentTexture, filter);
	rlTextureParameters(_currentTexture.id, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_CLAMP);
	rlTextureParameters(_currentTexture.id, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_CLAMP);
	//rlTextureParameters(_currentTexture.id, 0x8501, -1.0f); // 0x8501 = GL_TEXTURE_LOD_BIAS

	_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = _currentTexture;

	_pendingBytes.clear();
	_pendingUpdate = false;
}