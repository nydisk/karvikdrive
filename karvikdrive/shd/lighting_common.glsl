// for any fragment shader lit by environment lighting (sun)

uniform sampler2D shadowMap;

uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform vec3 ambientColor;

uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;

uniform vec3 cameraPos;

float shadowFactor(vec4 lightSpacePos, vec3 normal) {
    vec3 proj = lightSpacePos.xyz / lightSpacePos.w;
    proj = proj * 0.5 + 0.5;

    if (proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0 || proj.z > 1.0)
        return 0.0;

    float shadow = 0.0;
    float bias = max(0.005 * (1.0 - dot(normal, -sunDirection)), 0.001);
    vec2 texelSz = 1.0 / vec2(textureSize(shadowMap, 0));

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, proj.xy + vec2(x, y) * texelSz).r;
            shadow += proj.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

vec3 applyFog(vec3 color, vec3 worldPos) {
    float dist = length(worldPos - cameraPos);
    float fogFact = clamp((dist - fogNear) / (fogFar - fogNear), 0.0, 1.0);
    return mix(color, fogColor, fogFact * fogFact);
}
