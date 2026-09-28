#include "CameraController.hpp"
#include <raymath.h>
#include <algorithm>
#include <cmath>

CameraController::CameraController() {
    _cam.fovy = 55.0f;
    _cam.up = Vector3UnitY;
    _cam.position = Vector3Ones;
    _cam.target = Vector3Zeros;
    _cam.projection = CAMERA_PERSPECTIVE;
}

void CameraController::update(Vector3 carPos, float carDir, float dt) {
    if (IsCursorHidden()) {
        handleZoom();
        handleMouseLook(carDir, dt);
    }

    float yawRad = _yaw * DEG2RAD;
    float pitchRad = _pitch * DEG2RAD;

    _cam.position = {
        carPos.x + (-cosf(yawRad) * cosf(pitchRad) * _distance),
        carPos.y + (sinf(pitchRad) * _distance + 2.0f),
        carPos.z + (-sinf(yawRad) * cosf(pitchRad) * _distance)
    };
    _cam.target = { carPos.x, carPos.y + 1.0f, carPos.z };
}

Camera3D& CameraController::camera() { return _cam; }

const Camera3D& CameraController::camera() const { return _cam; }

void CameraController::snapToOrigin(float dx, float dz) {
    _cam.position.x -= dx;
    _cam.position.z -= dz;
    _cam.target.x -= dx;
    _cam.target.z -= dz;
}

void CameraController::handleZoom() {
    float scroll = GetMouseWheelMove();
    if (scroll != 0.0f) {
        _distance = std::clamp(_distance - scroll * kZoomSpeed, kDistanceMin, kDistanceMax);
        _idleTime = 0.0f;
    }
}

void CameraController::handleMouseLook(float carDir, float dt) {
    Vector2 delta = GetMouseDelta();
    float delta2 = delta.x * delta.x + delta.y * delta.y;

    if (delta2 > 0.01f) {
        _yaw += delta.x * 0.1f;
        _pitch = std::clamp(_pitch + delta.y * 0.1f, -10.0f, 45.0f);
        _idleTime = 0.0f;
    }
    else {
        _idleTime += dt;
        if (_idleTime > kRecenterDelay) {
            float diff = fmodf(carDir - _yaw + 540.0f, 360.0f) - 180.0f;
            _yaw += diff * std::min(dt * kRecenterSpeed, 1.0f);
        }
    }
}
