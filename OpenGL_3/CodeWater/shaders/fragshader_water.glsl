#version 330 core

// Define constants.
#define M_PI 3.141593

// The input from the vertex shader.
in vec2 uv;
in vec3 vertNormal;

// Illumination model constants.
//uniform vec4 material;
//uniform vec3 lightColor;

// Texture sampler.
//uniform sampler2D textureSampler;

// Specify the output of the fragment shader.
out vec4 fColor;

void main()
{
    // Normalize the interpolated normal and map it to a colour
    // Note that colours have the range [0, 1] and the normal [-1, 1]
    vec3 ones = vec3(1.0);
    fColor = vec4((normalize(vertNormal) + ones) * 0.5F, 1.0F);
}
