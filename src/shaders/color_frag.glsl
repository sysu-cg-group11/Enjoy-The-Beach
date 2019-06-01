#version 330 core
out vec4 FragColor;
in vec3 verColor;

void main()
{
    FragColor = vec4(verColor, 1.0);
}
