#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse0;
uniform vec3 sprite_color;
uniform int type;

void main() 
{
	vec3 ambient = 0.5 * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = 0.8 * diff * lightColor;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
	vec3 specular = 0.2 * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * sprite_color;

	if (type == 1) {
		FragColor = vec4(result, 1.0);
	} else {
		FragColor = vec4(result, 1.0) * texture(texture_diffuse0, TexCoords);
	}
}