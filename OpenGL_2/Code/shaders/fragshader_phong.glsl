#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
in vec3 lightPositionRel, vertexPos, N;

// Specify the Uniforms of the fragment shaders
uniform vec3 lightColor;
uniform vec3 materialColor;
uniform vec3 materialKs;

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
    vec3 ambient = materialColor * materialKs[0];
    vec3 diffuse = max(0.0, dot(N, L)) * lightColor * materialColor * materialKs[1];
    vec3 specular = pow(max(0.0, dot(R, V)), 1.0) * lightColor * materialKs[2];

    // sum of the shading components
    fNormal = vec4(ambient + diffuse + specular, 1.0);
}
