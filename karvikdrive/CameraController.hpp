#pragma once
#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <cmath>

class CameraController {
    Camera3D _cam{};
    float _yaw = 0.0f;
    float _pitch = DefaultPitch;
    float _distance = DefaultDistance;
    float _idleTime = 0.0f;

    static constexpr float DefaultPitch = 18.0f;
    static constexpr float DefaultDistance = 16.0f;
    static constexpr float DistanceMin = 4.0f;
    static constexpr float DistanceMax = 40.0f;
    static constexpr float RecenterDelay = 1.5f;
    static constexpr float RecenterSpeed = 3.0f;
    static constexpr float ZoomSpeed = 2.0f;
public:
    CameraController() {
        _cam.fovy = 55.0f;
        _cam.up = Vector3UnitY;
        _cam.position = Vector3Ones;
        _cam.target = Vector3Zeros;
        _cam.projection = CAMERA_PERSPECTIVE;
    }

    void update(Vector3 carPos, float carDir, float dt) {
        handleZoom();
        handleMouseLook(carDir, dt);

        float yawRad = _yaw * DEG2RAD;
        float pitchRad = _pitch * DEG2RAD;

        _cam.position = {
            carPos.x + (-cosf(yawRad) * cosf(pitchRad) * _distance),
            carPos.y + (sinf(pitchRad) * _distance + 2.0f),
            carPos.z + (-sinf(yawRad) * cosf(pitchRad) * _distance)
        };
        _cam.target = { carPos.x, carPos.y + 1.0f, carPos.z };
    }

    Camera3D& camera() { return _cam; }
    const Camera3D& camera() const { return _cam; }

    void snapToOrigin(float dx, float dz) {
        _cam.position.x -= dx;
        _cam.position.z -= dz;
        _cam.target.x -= dx;
        _cam.target.z -= dz;
    }
private:
    void handleZoom() {
        float scroll = GetMouseWheelMove();
        if (scroll != 0.0f) {
            _distance = std::clamp(_distance - scroll * ZoomSpeed, DistanceMin, DistanceMax);
            _idleTime = 0.0f;
        }
    }

    void handleMouseLook(float carDir, float dt) {
        Vector2 delta = GetMouseDelta();
        float delta2 = delta.x * delta.x + delta.y * delta.y;

        if (delta2 > 0.01f) {
            _yaw += delta.x * 0.1f;
            _pitch = std::clamp(_pitch + delta.y * 0.1f, -10.0f, 45.0f);
            _idleTime = 0.0f;
        }
        else {
            _idleTime += dt;
            if (_idleTime > RecenterDelay) {
                float diff = fmodf(carDir - _yaw + 540.0f, 360.0f) - 180.0f;
                _yaw += diff * std::min(dt * RecenterSpeed, 1.0f);
            }
        }
    }
};