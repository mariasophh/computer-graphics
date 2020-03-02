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
uniform vec3 lightCol;
uniform vec3 materialCol;
uniform vec3 materialK;

// Specify the output of the vertex stage
out vec3 vertNormal;
out vec2 textureCoordinates;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0);

    // Vertex position, normal and relative light position wrt scene
    vec3 vertexPosition = vec3(modelViewTransform * vec4(vertCoordinates_in, 1.0));
    vec3 N = normalize(normalTransform * vertNormal_in);
    vec3 relLightPosition = vec3(modelViewTransform * vec4(lightPos, 1.0));

    // Light, reflexion and view vectors
    vec3 L = normalize(relLightPosition - vertexPosition);
    vec3 R = 2 * dot(N, L) * N - L;
    vec3 V = normalize(vec3(0.0) - vertexPosition);

    vec3 ambient = materialCol * materialK[0];
    vec3 diffuse = lightCol * materialCol * materialK[1] * max(0.0, dot(N, L));
    vec3 specular = lightCol * materialK[2] * pow(max(0.0, dot(R, V)), 1.0);

    vertNormal = ambient + diffuse + specular;
    textureCoordinates = textureCoordinates_in;
}
