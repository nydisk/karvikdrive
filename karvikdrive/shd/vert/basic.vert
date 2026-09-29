#version 330 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 lightSpaceMatrix;
uniform float snapStrength;

out vec3 fragWorldPos;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec4 fragLightSpacePos;

#include "vertex_common.glsl"

void main() {
    fragTexCoord = vertexTexCoord;
    gl_Position = standardVertexTransform(
        vertexPosition, vertexNormal, matModel, mvp, lightSpaceMatrix, snapStrength,
        fragWorldPos, fragNormal, fragLightSpacePos
    );
}
