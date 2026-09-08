#version 330 core

    in vec2 TexCoord;
    flat in float Layer;

    out vec4 FragColor;

    uniform sampler2DArray texture1;

    void main()
    {
        FragColor = texture(texture1, vec3(TexCoord, Layer));
    }