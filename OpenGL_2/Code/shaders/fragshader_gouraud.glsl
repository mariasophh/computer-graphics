#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;
in vec2 textureCoordinates;

// Specify the Uniforms of the fragment shaders
uniform sampler2D samplerUniform;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

void main()
{
// normalize the interpolated normal and map it to a colour
// note that colours have the range [0, 1] and the normal [-1, 1]
    vec4 textureColor = texture(samplerUniform, textureCoordinates);
    fNormal = textureColor * vec4(vertNormal, 1.0);

}
