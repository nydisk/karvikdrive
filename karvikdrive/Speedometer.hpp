#pragma once
#include <raylib.h>
#include <raymath.h>
#include <string>
#include "Configuration.hpp"
#include "JSONHelper.hpp"

class Speedometer {
public:
    Speedometer() {
        _speedMeter = LoadTexture("assets/speed.png");
    }

    ~Speedometer() {
        UnloadTexture(_speedMeter);
    }

    void draw(Vector3 velocity) const {
        float uiScale = Configuration::graphicsCfg.uiScale * (Configuration::graphicsCfg.height / 1080.0f);
        float speedKmh = Vector3Length(velocity) * 3.6f;

        drawSpeedometer(speedKmh, uiScale);
    }

private:
    Texture2D _speedMeter{};

    void drawSpeedometer(float speedKmh, float uiScale) const {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        float mw = (float)_speedMeter.width * uiScale;
        float mh = (float)_speedMeter.height * uiScale;
        float mx = sw / 2.0f - mw / 2.0f;
        float my = sh - mh - 8.0f * uiScale;

        DrawRectangle(
            int(mx - 4.0f * uiScale),
            int(my - 4.0f * uiScale),
            int(mw + 4.0f * uiScale * 2.0f),
            int(mh + 4.0f * uiScale * 2.0f),
            { 0, 0, 0, 128 }
        );
        DrawTextureEx(_speedMeter, { mx, my }, 0.0f, uiScale, WHITE);

        float angle = Lerp(PI, 2.0f * PI, speedKmh / 300.0f);
        Vector2 base = { sw / 2.0f, sh - 4.0f * uiScale - 5.0f * uiScale };
        Vector2 end = base + Vector2{ cosf(angle), sinf(angle) } * 85.0f * uiScale;
        DrawLineEx(base, end, 5.0f * uiScale, RED);
    }
};