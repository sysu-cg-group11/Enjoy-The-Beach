#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 lightingColor;

uniform vec3 objectColor;


void main() {

    vec3 result = lightingColor * objectColor;
    FragColor = vec4(result, 1.0);
}
