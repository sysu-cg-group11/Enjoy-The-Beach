#version 330 core
layout (location = 0) in vec3 aPos;                                             
layout (location = 1) in vec2 aTexCoords;                                             
layout (location = 2) in vec3 aNormal;                                               
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[MAX_BONES];
uniform mat4 lightSpaceMatrix;

void main()
{       
    mat4 BoneTransform = gBones[aBoneIDs[0]] * aWeights[0];
    BoneTransform     += gBones[aBoneIDs[1]] * aWeights[1];
    BoneTransform     += gBones[aBoneIDs[2]] * aWeights[2];
    BoneTransform     += gBones[aBoneIDs[3]] * aWeights[3];


    vec4 position = BoneTransform * vec4(aPos, 1.0);
    
	vs_out.FragPos = vec3(model * position);
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	gl_Position  = projection * view * model * position;
}