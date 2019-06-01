#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform int shininess;

uniform sampler2D objectTex;
uniform sampler2D shadowMap;

uniform bool useTexture;
uniform bool pcf;
uniform bool useBias;


float calculateShadow(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closeDepth = texture(shadowMap, projCoords.xy).r;

    // Depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    vec3 normal = normalize(normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float thetaCos = dot(normal, lightDir);
    float bias = 0;
    if (useBias) {
        bias = max(0.05 * (1.0 - thetaCos), 0.005);
    }

    float shadow = 0;

    if (pcf) {
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;

                shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
            }
        }

        shadow /= 9.0f;
    } else {
        shadow = currentDepth - bias > closeDepth  ? 1.0 : 0.0;
    }

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

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

    float shadow = calculateShadow(FragPosLightSpace);
    vec3 result = (ambient + (1 - shadow) * (diffuse + specular));
    if (useTexture) {
        result *= texture(objectTex, TexCoord).rgb;
    } else {
        result *= objectColor;
    }
    FragColor = vec4(result, 1.0);
}
