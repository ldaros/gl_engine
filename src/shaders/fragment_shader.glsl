#version 410 core

// Output color of the fragment (pixel)
out vec4 FragColor;  // RGBA color for the fragment, where A is the alpha (opacity)

in vec3 vertexColor; // Input color from the vertex shader (passed through from vertex shader)

void main()
{
    // Set the output fragment color
    // Convert the vec3 vertex color to a vec4 (adding alpha as 1.0 for full opacity)
    FragColor = vec4(vertexColor, 1.0);
}