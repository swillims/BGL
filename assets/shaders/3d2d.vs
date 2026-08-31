#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float aDelta;

out vec2 TexCoord;

layout(std140) uniform view
{
    mat4 viewVar;
};

layout(std140) uniform projection
{
    mat4 projectionVar;
};

void main()
{
    vec3 cameraRight = vec3
    (
        viewVar[0][0],
        viewVar[1][0],
        viewVar[2][0]
    );

    cameraRight.y = 0.0;
    cameraRight = normalize(cameraRight);
    vec3 worldPosition = aPos;
    worldPosition += cameraRight * aDelta;
    gl_Position = projectionVar * viewVar * vec4(worldPosition, 1.0);

    TexCoord = aTexCoord;
}