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
    // normalize the interpolated normal and map it to a colour
    // note that colours have the range [0, 1] and the normal [-1, 1]
    vec3 ones = vec3(1.0);
    fNormal = vec4((normalize(vertNormal) + ones) * 0.5, 1.0);
}
