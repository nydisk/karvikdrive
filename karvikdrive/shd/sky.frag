#version 330 core
in vec2 fragTexCoord;
uniform vec3 horizonColor;
uniform vec3 zenithColor;
uniform vec3 sunDirection;
uniform float timeNorm;
uniform float screenWidth;
uniform float screenHeight;
uniform float bandWidth;
uniform float horizonScale;
uniform float zenithScale;
out vec4 finalColor;

void main() {
    vec2 uv = fragTexCoord;

    float t = pow(abs(uv.y - 0.5) * 2.0, bandWidth);
    vec3 sky = mix(horizonColor * horizonScale, zenithColor * zenithScale, t);

    vec2 screenUV  = vec2(uv.x, 1.0 - uv.y);
    vec2 sunScreen = vec2(
        0.5 + 0.5 * (-sunDirection.x / max(abs(sunDirection.x), 0.001)),
        1.0 - (sunDirection.y * 0.5 + 0.5)
    );
    float sunDist = length(screenUV - sunScreen);
    float sunDisc = smoothstep(0.04, 0.01, sunDist);
    sky += sunDisc * vec3(1.0, 0.95, 0.7) * 2.0;

    finalColor = vec4(clamp(sky, 0.0, 1.0), 1.0);
}
