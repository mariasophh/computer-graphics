#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 N, relLightPosition, vertexPosition;
in vec2 textureCoordinates;

// Specify the Uniforms of the fragment shaders
uniform vec3 materialCol;
uniform vec3 materialK;
uniform vec3 lightCol;

uniform sampler2D samplerUniform;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

void main()
{
    // Light, reflexion and view vectors
    vec3 L = normalize(relLightPosition - vertexPosition);
    vec3 R = 2 * dot(N, L) * N - L;
    vec3 V = normalize(vec3(0.0) - vertexPosition);

    vec3 ambient = materialCol * materialK[0];
    vec3 diffuse = lightCol * materialCol * materialK[1] * max(0.0, dot(N, L));
    vec3 specular = lightCol * materialK[2] * pow(max(0.0, dot(R, V)), 1.0);

    vec4 textureColor = texture(samplerUniform, textureCoordinates);
    fNormal = textureColor * vec4(ambient + diffuse + specular, 1.0);
}
