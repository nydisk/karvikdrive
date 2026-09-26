#pragma once
#include <cmath>

inline float r(float v, int decimals = 4) {
    float factor = powf(10.0f, (float)decimals);
    return roundf(v * factor) / factor;
}

inline double rd(double v, int decimals = 4) {
    double factor = pow(10.0, (double)decimals);
    return round(v * factor) / factor;
}