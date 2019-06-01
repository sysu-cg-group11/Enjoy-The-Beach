#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 aNormal;

out vec3 lightingColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform int shininess;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);

    vec3 normal = mat3(transpose(inverse(model))) * aNormal;
    vec3 FragPos = vec3(model * vec4(pos, 1.0));
    //embient
    vec3 ambient = ambientStrength * lightColor;

    //diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    lightingColor = (ambient + diffuse + specular);

}
