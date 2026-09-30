// should be included by any fragment shader that wants to receive headlight illumination

uniform vec3 headlightPosL;
uniform vec3 headlightPosR;
uniform vec3 headlightDir;
uniform vec3 headlightColor;
uniform float headlightRange;
uniform float headlightInnerCos;
uniform float headlightOuterCos;

vec3 headlightContribution(vec3 lightPos, vec3 worldPos, vec3 normal) {
    vec3 toLight = lightPos - worldPos;
    float dist = length(toLight);
    vec3 lightDirN = toLight / max(dist, 0.0001);

    float diff = max(dot(normal, lightDirN), 0.0);

    float spotCos = dot(-lightDirN, normalize(headlightDir));
    float spotFactor = clamp((spotCos - headlightOuterCos) / max(headlightInnerCos - headlightOuterCos, 0.0001), 0.0, 1.0);

    float atten = clamp(1.0 - dist / headlightRange, 0.0, 1.0);
    atten *= atten;

    return headlightColor * diff * spotFactor * atten;
}

vec3 headlightsTotal(vec3 worldPos, vec3 normal) {
    return headlightContribution(headlightPosL, worldPos, normal) + headlightContribution(headlightPosR, worldPos, normal);
}
