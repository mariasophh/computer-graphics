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

uniform vec3 lightPosition;

// Specify the output of the vertex stage
out vec3 lightPositionRel, vertexPos, N;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);

    // relative light position (fix it wrt the scene)
    lightPositionRel = vec3(modelViewTransform * vec4(lightPosition, 1.0));
    // vertex position and vertex normal
    vertexPos = vec3(modelViewTransform * vec4(vertCoordinates_in, 1.0));
    N = normalize(normalTransform * vertNormal_in);
}
