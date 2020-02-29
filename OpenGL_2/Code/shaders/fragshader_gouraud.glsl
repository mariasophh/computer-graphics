#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
in float ambient, diffuse, specular;
in vec2 textureCoords;

// Specify the uniforms of the fragment shader
uniform sampler2D samplerUniform;
uniform vec3 materialColor;
uniform vec3 lightColor;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

void main()
{
    vec4 textureColor = texture(samplerUniform, textureCoords);
    fNormal = ambient * textureColor + (diffuse + specular) * (lightColor, 1.0) * textureColor;

    // WITHOUT TEXTURE
//    vec3 color = ambient * materialColor + diffuse * materialColor * lightColor + specular * lightColor;
//    fNormal = vec4(color, 1.0);
}
