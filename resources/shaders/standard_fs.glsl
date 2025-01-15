#version 410 core

// Input/Output
in VS_OUT
{
    vec2 UV;
    vec3 Position_worldspace;
    vec3 Normal_cameraspace;
    vec3 EyeDirection_cameraspace;
    vec3 LightDirection_cameraspace;
    vec3 Tangent_cameraspace;
    vec3 Bitangent_cameraspace;
    vec4 Position_lightspace;
} fs_in;

out vec4 FragColor;

// Uniforms
uniform sampler2D textureDiffuse;
uniform sampler2D textureNormal;
uniform sampler2D shadowMap;
uniform bool useNormalMap = false;

uniform vec3 lightColor = vec3(1.0);
uniform float lightPower = 50.0;
uniform vec3 materialAmbient = vec3(0.1);
uniform vec3 materialSpecular = vec3(0.3);
uniform float shininess = 32.0;
uniform float opacity = 1.0;
uniform bool isDirectionalLight = false;

float calculateShadow()
{
    if (!isDirectionalLight) return 0.0;

    vec3 projCoords = fs_in.Position_lightspace.xyz / fs_in.Position_lightspace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    vec3 normal = normalize(fs_in.Normal_cameraspace);
    vec3 lightDir = normalize(fs_in.LightDirection_cameraspace);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 calculateLighting(vec3 diffuseColor, vec3 normal)
{
    // Normalize vectors
    vec3 N = normalize(normal);
    vec3 L = normalize(fs_in.LightDirection_cameraspace);
    vec3 E = normalize(fs_in.EyeDirection_cameraspace);
    vec3 R = reflect(-L, N);

    // Calculate light attenuation
    float attenuation = 1.0;
    if (!isDirectionalLight)
    {
        // Only calculate distance attenuation for point lights
        float distance = length(fs_in.LightDirection_cameraspace);
        attenuation = 1.0 / (distance * distance);
    }
    
    // Calculate lighting components
    vec3 ambient = materialAmbient * diffuseColor;
    vec3 diffuse = diffuseColor * lightColor * lightPower * max(dot(N, L), 0.0) * attenuation;
    vec3 specular = materialSpecular * lightColor * lightPower * pow(max(dot(E, R), 0.0), shininess) * attenuation;

    float shadow = calculateShadow();
    return ambient + (1.0 - shadow) * (diffuse + specular);
}

void main()
{
    vec3 diffuseColor = texture(textureDiffuse, fs_in.UV).rgb;

    // Determine wich normal to use
    vec3 normal;

    if (useNormalMap)
    {
        // Sample and process normal map
        vec3 normalMapColor = texture(textureNormal, fs_in.UV).rgb;
        vec3 TBNNormal = normalize(normalMapColor * 2.0 - 1.0);
        
        // Construct TBN matrix and transform normal
        mat3 TBN = mat3(fs_in.Tangent_cameraspace, 
                        fs_in.Bitangent_cameraspace, 
                        fs_in.Normal_cameraspace);
        normal = normalize(TBN * TBNNormal);
    } else {
        // Use the interpolated vertex normal
        normal = normalize(fs_in.Normal_cameraspace);
    }

    vec3 litColor = calculateLighting(diffuseColor, normal);
    FragColor = vec4(litColor, opacity);
}