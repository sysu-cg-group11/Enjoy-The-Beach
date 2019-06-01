#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform int shininess;

void main() {
    //embient
    vec3 ambient = ambientStrength * lightColor;

    //diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 mid = normalize((lightDir + viewDir));
    float spec = pow(max(dot(norm, mid), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
