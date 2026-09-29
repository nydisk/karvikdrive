// vertex transform for standard lit meshes: world pos, normal, shadow-space pos, and clip-space position with optional pixel snapping
// nightmare parameter soup

vec4 standardVertexTransform(
    vec3 vertexPosition,
    vec3 vertexNormal,
    mat4 matModel,
    mat4 mvp,
    mat4 lightSpaceMatrix,
    float snapStrength,
    out vec3 worldPos,
    out vec3 normal,
    out vec4 lightSpacePos
) {
    vec4 worldPos4 = matModel * vec4(vertexPosition, 1.0);
    worldPos = worldPos4.xyz;
    normal = normalize(mat3(matModel) * vertexNormal);
    lightSpacePos = lightSpaceMatrix * worldPos4;

    vec4 clipPos = mvp * vec4(vertexPosition, 1.0);

    if (snapStrength > 0.0) {
        float snapRes = mix(1.0, 128.0, snapStrength);
        clipPos.xy = floor(clipPos.xy / clipPos.w * snapRes + 0.5) / snapRes * clipPos.w;
    }

    return clipPos;
}
