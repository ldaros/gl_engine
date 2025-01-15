#version 410 core

layout(location = 0) in vec3 vertexPosition; // Vertex position attribute

uniform mat4 lightSpaceMatrix; // Matrix to transform the position to light space
uniform mat4 model; // Model matrix

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(vertexPosition, 1.0); // Transform to light space
}