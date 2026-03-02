#version 330 core
 in vec2 TexCoord;
 out vec4 color;
 uniform sampler2D texture1;
 uniform vec3 textColor;
 void main()
 {
	color = vec4(textColor, 1.0) * vec4(1.0,1.0,1.0, texture(texture1, TexCoord).r);
 }