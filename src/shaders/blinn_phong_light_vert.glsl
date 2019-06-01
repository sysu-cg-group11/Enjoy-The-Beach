#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    FragPos = vec3(model * vec4(pos, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal;
}
