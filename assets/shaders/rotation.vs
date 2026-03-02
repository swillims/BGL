#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform float u_rotation;          // radians
uniform vec2 rotation_center;
uniform float aspect_ratio;

out vec2 TexCoord;

void main()
{
    float c = cos(u_rotation);
    float s = sin(u_rotation);

    vec2 pos = aPos.xy - rotation_center; // offset position to center object(for rotation)
    pos.x *= aspect_ratio; // change size to compensate for aspect ratio related size distortion
    vec2 rotated = vec2(
        pos.y * s - pos.x * c,
        pos.x * s + pos.y * c
    );
    rotated.x /= aspect_ratio; // dividiing by aspect ratio removes rotation distortion but creates size change related distortion
    rotated += rotation_center; // un-offset position to not be center

    gl_Position = vec4(rotated, aPos.z, 1.0);
    TexCoord = aTexCoord;
}