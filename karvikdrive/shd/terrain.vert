#version 330 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 matModel;

uniform float snapStrength;
uniform vec2 screenSize;

out vec3 fragWorldPos;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec4 fragLightSpacePos;

uniform mat4 lightSpaceMatrix;

void main() {
    vec4 worldPos4 = matModel * vec4(vertexPosition, 1.0);
    fragWorldPos = worldPos4.xyz;
    fragTexCoord = vertexTexCoord;
    fragNormal = normalize(mat3(matModel) * vertexNormal);
    fragLightSpacePos = lightSpaceMatrix * worldPos4;

    vec4 clipPos = mvp * vec4(vertexPosition, 1.0);

    if (snapStrength > 0.0) {
        float snapRes = mix(1.0, 128.0, snapStrength);
        clipPos.xy = floor(clipPos.xy / clipPos.w * snapRes + 0.5) / snapRes * clipPos.w;
    }

    gl_Position = clipPos;
}