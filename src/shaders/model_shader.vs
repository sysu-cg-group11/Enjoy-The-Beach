#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aOffset;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

uniform int type;

uniform vec4 plane;

void main()
{	
	if(type == 0) {
		vec4 worldPosition = model * vec4(aPos + aOffset, 1.0f);
		gl_ClipDistance[0] = dot(worldPosition, plane);

		gl_Position = projection * view * model * vec4(aPos + aOffset, 1.0f);
	    vs_out.FragPos = vec3(model * vec4(aPos + aOffset, 1.0f));
	    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
	    vs_out.TexCoords = aTexCoords;
	    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0f);
	}
	else {
		vec4 worldPosition = model * vec4(aPos, 1.0f);
		gl_ClipDistance[0] = dot(worldPosition, plane);

		gl_Position = projection * view * model * vec4(aPos, 1.0f);
		vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
		vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
		vs_out.TexCoords = aTexCoords;
		vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0f);
	}

}