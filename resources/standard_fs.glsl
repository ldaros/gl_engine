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
} fs_in;

out vec4 FragColor;

// Uniforms
uniform sampler2D textureDiffuse;
uniform sampler2D textureNormal;
uniform bool useNormalMap = false;

uniform vec3 lightColor = vec3(1.0);
uniform float lightPower = 50.0;
uniform vec3 materialAmbient = vec3(0.1);
uniform vec3 materialSpecular = vec3(0.3);
uniform float shininess = 32.0;
uniform float opacity = 1.0;

vec3 calculateLighting(vec3 diffuseColor, vec3 normal)
{
    // Normalize vectors
    vec3 N = normalize(normal);
    vec3 L = normalize(fs_in.LightDirection_cameraspace);
    vec3 E = normalize(fs_in.EyeDirection_cameraspace);
    vec3 R = reflect(-L, N);

    // Calculate light attenuation
    float distance = length(fs_in.LightDirection_cameraspace);
    float attenuation = 1.0 / (distance * distance);

    // Calculate lighting components
    vec3 ambient = materialAmbient * diffuseColor;
    vec3 diffuse = diffuseColor * lightColor * lightPower * max(dot(N, L), 0.0) * attenuation;
    vec3 specular = materialSpecular * lightColor * lightPower * pow(max(dot(E, R), 0.0), shininess) * attenuation;

    return ambient + diffuse + specular;
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