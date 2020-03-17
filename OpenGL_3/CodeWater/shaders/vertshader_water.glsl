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
uniform mat3 normalTransform;

uniform float amplitude[5];
uniform float frequency[5];
uniform float phase[5];

uniform float time;

// Specify the output of the vertex stage.
//out vec2 uv;
out vec3 waveNormal;
out vec3 currentPosition;
out vec3 materialColor;
out vec3 camera;

// Calculate z coord. based on u: f (u, t) = A * sin(2πf * u + t + φ)
float waveHeight(int waveIdx, float u) {
    return amplitude[waveIdx] * sin(2.0 * M_PI * frequency[waveIdx] * u + phase[waveIdx] + time);
}

// Calculate derivative: f'(u, t) = A * 2πf * cos(2πf * u + t + φ)
float waveDU(int waveIdx, float u) {
    return amplitude[waveIdx] * 2.0 * M_PI * frequency[waveIdx] * cos(2.0 * M_PI * frequency[waveIdx] * u + phase[waveIdx] + time);
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

    currentPosition = vec3(vertCoordinates_in.x, vertCoordinates_in.y, height);
    gl_Position     = projectionTransform * modelViewTransform * vec4(currentPosition, 1.0F);

    // Material color is derived from final wave height
    float Hermite_interpolation = smoothstep(-0.5, 0.5, height);
    vec3 low_color  = vec3(0, 0, 1); // blue color for low parts
    vec3 high_color = vec3(1, 1, 1); // white color for low parts

    // Pass the required information to the fragment shader stage.
    waveNormal            = normalTransform * normalize(vec3(-normal, -0, 1.0F)); //dV = 0
    materialColor         = mix(low_color, high_color, Hermite_interpolation);
    camera                = vec3(projectionTransform[3] / projectionTransform[3].w);
    //uv         = uvCoords_in;
}
