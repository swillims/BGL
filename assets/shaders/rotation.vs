#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec2 rotation_center;
layout(location = 3) in vec2 rotation_data;

out vec2 TexCoord;

void main()
{
    float u_rotation = rotation_data.x;
    float aspect_ratio = rotation_data.y;

    float c = cos(u_rotation);
    float s = sin(u_rotation);

    vec2 pos = aPos - rotation_center; // offset position to center object

    pos.x *= aspect_ratio; // compensate for aspect ratio distortion

    vec2 rotated = vec2(
        pos.y * s - pos.x * c,
        pos.x * s + pos.y * c
    );

    rotated.x /= aspect_ratio; // undo rotation distortion

    rotated += rotation_center;

    gl_Position = vec4(rotated, 0.0, 1.0);
    TexCoord = aTexCoord;
}