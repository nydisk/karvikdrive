#pragma once
#include <raylib.h>

class CameraController {
    Camera3D _cam{};
    float _yaw = 0.0f;
    float _pitch = kDefaultPitch;
    float _distance = kDefaultDistance;
    float _idleTime = 0.0f;

    static constexpr float kDefaultPitch = 18.0f;
    static constexpr float kDefaultDistance = 16.0f;
    static constexpr float kDistanceMin = 4.0f;
    static constexpr float kDistanceMax = 40.0f;
    static constexpr float kRecenterDelay = 1.5f;
    static constexpr float kRecenterSpeed = 3.0f;
    static constexpr float kZoomSpeed = 2.0f;
public:
    CameraController();

    void update(Vector3 carPos, float carDir, float dt);

    Camera3D& camera();
    const Camera3D& camera() const;

    void snapToOrigin(float dx, float dz);
private:
    void handleZoom();

    void handleMouseLook(float carDir, float dt);
};