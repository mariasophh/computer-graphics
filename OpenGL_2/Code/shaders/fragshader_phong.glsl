#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
in vec3 lightPositionRel, vertexPos, N;
in vec2 textureCoords;

// Specify the Uniforms of the fragment shaders
uniform vec3 lightColor;
uniform vec3 materialColor;
uniform vec3 materialKs;
uniform sampler2D samplerUniform;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

void main()
{
    // normalized light vector
    vec3 L = normalize(lightPositionRel - vertexPos);

    // normalized reflection vector
    vec3 R = 2 * dot(N, L) * N - L;
    // normalized view vector, assuming camera is at origin
    vec3 V = normalize(vec3(0.0) - vertexPos);

    // compute shading components
    // note that KA = materialKs[0], KD = materialKs[1], KS = materialKs[2]
    float ambient = materialKs[0];
    float diffuse = max(0.0, dot(N, L)) * materialKs[1];
    float specular = pow(max(0.0, dot(R, V)), 1.0)  * materialKs[2]; // material normal 1.0

    vec4 textureColor = texture(samplerUniform, textureCoords);
    fNormal = ambient * textureColor + (diffuse + specular) * (lightColor, 1.0) * textureColor;

    // WITHOUT TEXTURE
//    vec3 color = ambient * materialColor + diffuse * materialColor * lightColor + specular * lightColor;
//    fNormal = vec4(color, 1.0);
}
