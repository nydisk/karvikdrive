#pragma once
#include <raylib.h>
#include <raymath.h>
#include <string>
#include "Configuration.hpp"
#include "JSONHelper.hpp"

class HUD {
public:
    HUD() {
        _speedMeter = LoadTexture("assets/speed.png");
    }

    ~HUD() {
        UnloadTexture(_speedMeter);
    }

    void draw(Vector3 velocity, double lat, double lon, double dist) const {
        float uiScale = Configuration::graphicsCfg.uiScale;
        float speedKmh = Vector3Length(velocity) * 3.6f;

        drawInfoBox(speedKmh, lat, lon, uiScale, dist);
        drawSpeedometer(speedKmh, uiScale);
    }

private:
    Texture2D _speedMeter{};

    void drawInfoBox(float speedKmh, double lat, double lon, float uiScale, double distanceTravelled) const {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << (int)speedKmh << " km/h\n";
        ss << distanceTravelled / 1000.0 << " km\n";
        ss << std::fixed << std::setprecision(6);
        ss << "lat " << lat << '\n';
        ss << "lon " << lon << '\n';
        std::string text = ss.str();

        const int padding = 4;
        int textW = MeasureText(text.c_str(), 20);
        DrawRectangle(
            8 - padding,
            8 - padding,
            int((textW + padding * 2.0f) * uiScale),
            int((20.0f * 4.0f + padding * 2.0f) * uiScale),
            { 0, 0, 0, 128 }
        );
        DrawText(text.c_str(), 8, 8, int(20 * uiScale), WHITE);
    }

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