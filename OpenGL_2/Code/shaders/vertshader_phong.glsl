#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 textureCoordinates_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;

uniform vec3 lightPos;

// Specify the output of the vertex stage
out vec3 N, relLightPosition, vertexPosition;
out vec2 textureCoordinates;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);

    relLightPosition = vec3(modelViewTransform * vec4(lightPos, 1.0));
    vertexPosition = vec3(modelViewTransform * vec4(vertCoordinates_in, 1.0));

    N = normalize(normalTransform * vertNormal_in);
    textureCoordinates = textureCoordinates_in;
}
