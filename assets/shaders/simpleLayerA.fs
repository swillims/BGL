#version 330 core

    in vec2 TexCoord;
    flat in float Layer;

    out vec4 FragColor;

    uniform sampler2DArray texture1;

    void main()
    {
        vec4 color = texture(texture1, vec3(TexCoord, Layer));
        if (color.a == 0.0)
        {
            discard;
        }
        FragColor = color;
    }