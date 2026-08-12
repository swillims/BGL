#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

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
    gl_Position = projectionVar * viewVar * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}