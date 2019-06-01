#version 330 core
out vec4 FragColor;
in vec3 verColor;
in vec2 texCoord;

uniform sampler2D texture_1;

void main()
{
    FragColor = texture(texture_1, texCoord) * vec4(verColor, 1.0);
}
