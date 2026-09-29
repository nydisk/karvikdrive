#version 330 core

in vec3 fragWorldPos;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec4 fragLightSpacePos;

uniform sampler2D texture0;
uniform vec3 viewPos;

#include "lighting_common.glsl"
#include "headlights.glsl"

out vec4 finalColor;

void main() {
    vec4 albedo = texture(texture0, fragTexCoord);
    float diff = max(dot(fragNormal, -sunDirection), 0.0);
    float shadow = shadowFactor(fragLightSpacePos, fragNormal);
    float lit = 1.0 - shadow * 0.85;

    vec3 viewDir = normalize(viewPos - fragWorldPos);
    vec3 halfDir = normalize(-sunDirection + viewDir);
    float spec = pow(max(dot(fragNormal, halfDir), 0.0), 32.0) * lit;

    vec3 lighting = ambientColor + (sunColor * diff + sunColor * spec * 0.3) * lit;
    lighting += headlightsTotal(fragWorldPos, fragNormal);
    vec3 color = albedo.rgb * clamp(lighting, 0.0, 1.5);

    color = applyFog(color, fragWorldPos);
    finalColor = vec4(color, albedo.a);
}
