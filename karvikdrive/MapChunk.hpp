#pragma once
#include <raylib.h>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>

struct MapChunkCoords { int x, y; };

class MapChunk {
    MapChunkCoords _coords;
    int _tileX, _tileY;
    double& _mpt;
    Mesh _mesh{};
    Model _model{};
    Texture2D _currentTexture{};

    std::atomic<bool> _pendingUpdate{ false };
    std::vector<uint8_t> _pendingBytes;
    std::mutex _mutex;
public:
    MapChunk(MapChunkCoords coords, int tileX, int tileY, double& mpt);
    void render(Shader* overrideShader);
    void renderDebug();
    void updateGridCoord(int centerTileX, int centerTileY);
    void uploadIfReady(int filter = TEXTURE_FILTER_ANISOTROPIC_8X);
    MapChunkCoords coords() const;
    void setShader(Shader shader);
private:
    Vector3 getPosition();
    void updateTexture();
};