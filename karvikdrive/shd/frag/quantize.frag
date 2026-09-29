#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform int colorDepth;
uniform float ditherStrength;
uniform float rw;
uniform float rh;

out vec4 finalColor;

float bayer4x4(ivec2 p) {
    const float m[16] = float[](
         0.0/16.0,  8.0/16.0,  2.0/16.0, 10.0/16.0,
        12.0/16.0,  4.0/16.0, 14.0/16.0,  6.0/16.0,
         3.0/16.0, 11.0/16.0,  1.0/16.0,  9.0/16.0,
        15.0/16.0,  7.0/16.0, 13.0/16.0,  5.0/16.0
    );
    return m[(p.y % 4) * 4 + (p.x % 4)];
}

vec3 quantize(vec3 color, float rLevels, float gLevels, float bLevels) {
    return vec3(
        floor(color.r * rLevels + 0.5) / rLevels,
        floor(color.g * gLevels + 0.5) / gLevels,
        floor(color.b * bLevels + 0.5) / bLevels
    );
}

void main() {
    vec3 color = texture(texture0, fragTexCoord).rgb;
    if (colorDepth > 0) {
        
        float levels = float(colorDepth);
        float stepSize = 1.0 / levels;

        ivec2 pix = ivec2(fragTexCoord * vec2(rw, rh));
        float threshold = (bayer4x4(pix) - 0.5) * ditherStrength;
        color = floor(color * levels + threshold) / levels;
    }
    finalColor = vec4(color, 1.0);
}