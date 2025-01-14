#version 410 core

// Input/Output
in VS_OUT {
    vec2 UV;
    vec3 Position_worldspace;
    vec3 Normal_cameraspace;
    vec3 EyeDirection_cameraspace;
    vec3 LightDirection_cameraspace;
} fs_in;

out vec4 FragColor;

// Uniforms
uniform sampler2D texture1;
uniform vec3 lightPosition;
uniform vec3 lightColor = vec3(1.0);
uniform float lightPower = 50.0;
uniform vec3 materialAmbient = vec3(0.1);
uniform vec3 materialSpecular = vec3(0.3);
uniform float shininess = 32.0;

vec3 calculateLighting(vec3 diffuseColor) {
    // Normalize vectors
    vec3 N = normalize(fs_in.Normal_cameraspace);
    vec3 L = normalize(fs_in.LightDirection_cameraspace);
    vec3 E = normalize(fs_in.EyeDirection_cameraspace);
    vec3 R = reflect(-L, N);

    // Calculate light attenuation
    float distance = length(lightPosition - fs_in.Position_worldspace);
    float attenuation = 1.0 / (distance * distance);

    // Calculate lighting components
    vec3 ambient = materialAmbient * diffuseColor;
    vec3 diffuse = diffuseColor * lightColor * lightPower * max(dot(N, L), 0.0) * attenuation;
    vec3 specular = materialSpecular * lightColor * lightPower * pow(max(dot(E, R), 0.0), shininess) * attenuation;

    return ambient + diffuse + specular;
}

void main() {
    vec4 textureColor = texture(texture1, fs_in.UV);
    vec3 diffuseColor = textureColor.rgb;
    vec3 litColor = calculateLighting(diffuseColor);
    FragColor = vec4(litColor, textureColor.a);
}