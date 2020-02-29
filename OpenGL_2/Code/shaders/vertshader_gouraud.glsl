#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;

uniform vec3 materialColor;
uniform vec3 materialKs;
uniform vec3 lightPosition;
uniform vec3 lightColor;

// Specify the output of the vertex stage
out vec3 vertNormal;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);

    // vertex position and vertex normal
    vec3 vertexPos = vec3(modelViewTransform * vec4(vertCoordinates_in, 1.0));
    vec3 N = normalize(normalTransform * vertNormal_in);

    // relative light position (fix it wrt the scene)
    vec3 lightPositionRel = vec3(modelViewTransform * vec4(lightPosition, 1.0));
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
    vertNormal = ambient + diffuse + specular;
}
