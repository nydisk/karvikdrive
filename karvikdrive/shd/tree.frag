#version 330 core
in vec2 fragTexCoord;
in vec3 fragWorldPos;

uniform sampler2D texture0;
uniform vec3 ambientColor;
uniform vec3 sunColor;
uniform vec3 sunDirection;
uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;
uniform vec3 cameraPos;

out vec4 finalColor;

void main() {
    vec4 albedo = texture(texture0, fragTexCoord);
    //TODO: remove this, seems to be some sort of debugging thing left over
    finalColor = vec4(1.0, 0.0, 0.0, 1.0); // solid red, ignore texture 
}
