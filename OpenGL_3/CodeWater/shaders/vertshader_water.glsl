#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes.
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormals_in;
layout (location = 2) in vec2 uvCoords_in;

// Specify the uniforms of the vertex shader.
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
//uniform vec3 lightPosition;
uniform mat3 normalTransform;
uniform float amplitude[5];
uniform float frequency[5];
uniform float phase[5];

// Specify the output of the vertex stage.

//out vec3 vertPosition;
//out vec3 relativeLightPosition;
out vec2 uv;
out vec3 vertNormal;

// Calculate z coord. based on u: f (u, t) = A * sin(2πf * u + t + φ)
float waveHeight(int waveIdx, float u) {
    return amplitude[waveIdx] * sin(2.0 * M_PI * frequency[waveIdx] * u + phase[waveIdx]);
}

// Calculate derivative: f'(u, t) = A * 2πf * cos(2πf * u + t + φ)
float waveDU(int waveIdx, float u) {
    return amplitude[waveIdx] * 2.0 * M_PI * frequency[waveIdx] * cos(2.0 * M_PI * frequency[waveIdx] * u + phase[waveIdx]);
}

void main()
{
    float height = 0;
    float normal = 0;
    // Loop over all waves and sum the results
    for (int i = 0; i < 5; ++i) {
        height += waveHeight(i, uvCoords_in.x);
        normal += waveDU(i, uvCoords_in.x);
    }

    vec3 currentPosition = vec3(vertCoordinates_in.x, vertCoordinates_in.y, height);
    gl_Position  = projectionTransform * modelViewTransform * vec4(currentPosition, 1.0F);

    vertNormal = normalize(normalTransform * (vec3(-normal, 0, 1.0F))); //dV = 0
    uv         = uvCoords_in;

    // Pass the required information to the fragment shader stage.
    //relativeLightPosition = vec3(modelViewTransform * vec4(lightPosition, 1.0F));
    //vertPosition = vec3(modelViewTransform * vec4(vertCoordinates_in, 1.0F));
    //vertNormal   = normalize(normalTransform * vertNormals_in);
}
