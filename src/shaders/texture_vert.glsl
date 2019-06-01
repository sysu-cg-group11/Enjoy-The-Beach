#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 textureCoord;

out vec3 verColor;
out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    verColor = vec3(1.0, 1.0, 1.0);
    texCoord = vec2(textureCoord.x, textureCoord.y);
}
