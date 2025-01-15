#version 410 core

// Input attributes
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

out VS_OUT 
{
	vec2 UV;
	vec3 Position_worldspace;
	vec3 Normal_cameraspace;
	vec3 EyeDirection_cameraspace;
	vec3 LightDirection_cameraspace;
	vec3 Tangent_cameraspace;
	vec3 Bitangent_cameraspace;
} vs_out;

// Uniforms
uniform mat4 modelViewProjection;	
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;

void main()
{
	vec4 vertexPos4 = vec4(vertexPosition, 1.0);

	gl_Position = modelViewProjection * vertexPos4;
	vs_out.Position_worldspace = (modelMatrix * vertexPos4).xyz;

	vec3 vertexPosition_cameraspace = (viewMatrix * modelMatrix * vertexPos4).xyz;
	vs_out.EyeDirection_cameraspace = -vertexPosition_cameraspace;

	vec3 lightPosition_cameraspace = (viewMatrix * vec4(lightPosition, 1.0)).xyz;
	vs_out.LightDirection_cameraspace = lightPosition_cameraspace + vs_out.EyeDirection_cameraspace;

    vs_out.Normal_cameraspace = normalize(normalMatrix * vertexNormal);
    vs_out.Tangent_cameraspace = normalize(normalMatrix * vertexTangent);
    vs_out.Bitangent_cameraspace = normalize(normalMatrix * vertexBitangent);

	vs_out.UV = vertexUV;
}