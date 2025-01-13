#version 410 core

in vec2 TexCoords;
in vec3 Normal;
out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoords); // Apply the texture
}