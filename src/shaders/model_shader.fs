#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D texture_diffuse0;
uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;
uniform float diffuseFactor;
uniform vec3 sprite_color;

uniform int gammaMode;

uniform int type;


float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0){
        shadow = 0.0;
    }
    return shadow;
}

void main()
{

    vec3 normal = normalize(fs_in.Normal);

    vec3 ambient =  ambientStrength * lightColor;

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor * diffuseFactor;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = spec * lightColor * specularStrength;
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);

    if(type == 0) {
        vec4 texColor = texture(diffuseTexture, fs_in.TexCoords);
        if(texColor.a < 0.1)
            discard;
        float gamma = 2.2;
        vec3 objectColor;
        if (gammaMode == 0)
            objectColor = pow(texture(diffuseTexture, fs_in.TexCoords).rgb, vec3(gamma));
        else
            objectColor = texture(diffuseTexture, fs_in.TexCoords).rgb;

        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;
        FragColor = vec4(lighting, 1.0);
    }
    else {

        vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * sprite_color;
        if(type == 1) {
            FragColor = vec4(result, 1.0);
        }
        else {
            FragColor = vec4(result, 1.0) * texture(texture_diffuse0, fs_in.TexCoords);
        }
        
    }

}