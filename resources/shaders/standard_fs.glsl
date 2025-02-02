#version 450 core

#define MAX_LIGHTS 10
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1

struct Light
{
    vec4 position;
    vec4 direction;
    vec4 color;
    vec4 power_type;
};

layout(std140, binding = 0) uniform LightsUBO
{
    Light lights[MAX_LIGHTS];
};
uniform int activeLights = 0;

// Input/Output
in VS_OUT
{
    vec2 UV;
    vec3 Position_worldspace;
    vec3 Normal_cameraspace;
    vec3 EyeDirection_cameraspace;
    vec3 Tangent_cameraspace;
    vec3 Bitangent_cameraspace;
} fs_in;

out vec4 FragColor;

// Uniforms
uniform mat4 viewMatrix;

uniform sampler2D textureAlbedo;
uniform sampler2D textureNormal;
uniform sampler2D textureSpecular;
uniform vec3 materialAmbient;
uniform vec3 specularStrength;
uniform float shininess;
uniform float opacity;

vec3 calculateLightContribution(Light light, vec3 diffuseColor, vec3 normal, vec3 specularColor)
{   
    vec3 lightColor = light.color.xyz;
    float lightPower = light.power_type.x;
    int lightType = int(light.power_type.y);

    // Calculate light direction in camera space
    vec3 lightDir;
    vec3 lightPosition_cameraspace;
    
    if(lightType == POINT_LIGHT)
    {
        lightPosition_cameraspace = (viewMatrix * light.position).xyz;
        lightDir = normalize(lightPosition_cameraspace + fs_in.EyeDirection_cameraspace);
    } 
    else
    {
        lightDir = normalize((viewMatrix * light.direction).xyz);
    }

    // Calculate attenuation
    float attenuation = 1.0;
    if(lightType == POINT_LIGHT)
    {
        float distance = length(light.position.xyz - fs_in.Position_worldspace);
        attenuation = 1.0 / (distance * distance);
    }

    // Lighting calculations
    vec3 N = normalize(normal);
    vec3 L = normalize(lightDir);
    vec3 E = normalize(fs_in.EyeDirection_cameraspace);
    vec3 R = reflect(-L, N);

    // Diffuse component
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = lightColor * lightPower * diff * diffuseColor * attenuation;

    // Specular component
    float spec = pow(max(dot(E, R), 0.0), shininess);
    vec3 specular = lightColor * lightPower * spec * specularColor * attenuation;

    return 1.0 * (diffuse + specular);
}

void main()
{
    vec3 diffuseColor = texture(textureAlbedo, fs_in.UV).rgb;
    vec3 specularColor = texture(textureSpecular, fs_in.UV).rgb * specularStrength;

    // Normal calculation
    vec3 normalMapColor = texture(textureNormal, fs_in.UV).rgb;
    vec3 TBNNormal = normalize(normalMapColor * 2.0 - 1.0);
    mat3 TBN = mat3(
        normalize(fs_in.Tangent_cameraspace),
        normalize(fs_in.Bitangent_cameraspace),
        normalize(fs_in.Normal_cameraspace)
    );
    vec3 normal = normalize(TBN * TBNNormal);

    // Ambient component
    vec3 result = materialAmbient * diffuseColor;

    // Accumulate light contributions
    int numLights = min(activeLights, MAX_LIGHTS);
    for(int i = 0; i < numLights; i++) 
    {
        result += calculateLightContribution(lights[i], diffuseColor, normal, specularColor);
    }

    FragColor = vec4(result, opacity);
}