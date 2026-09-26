#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in mat4 instanceTransform;

uniform mat4 mvp;

out vec2 fragTexCoord;
out vec3 fragWorldPos;

void main() {
    fragTexCoord = vertexTexCoord;
    fragWorldPos = vec3(instanceTransform * vec4(vertexPosition, 1.0));
    gl_Position = mvp * instanceTransform * vec4(vertexPosition, 1.0);
}