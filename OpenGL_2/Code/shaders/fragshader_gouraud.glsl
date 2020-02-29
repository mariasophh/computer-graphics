#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
in vec3 vertNormal;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

void main()
{
    fNormal = vec4(vertNormal, 1.0);
}
