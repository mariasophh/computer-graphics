#version 330 core

// Define constants.
#define M_PI 3.141593

// The input from the vertex shader.
//in vec2 uv;
in vec3 waveNormal;
in vec3 currentPosition;
in vec3 materialColor;
in vec3 camera;

// Illumination model constants.
uniform vec4 material;
uniform vec3 lightColor;
uniform vec3 lightPosition;

// Specify the output of the fragment shader.
out vec4 fColor;

void main()
{
    // Ambient color does not depend on any vectors.
    vec3 color    = material.x * materialColor;

    // Calculate light direction vectors in the Phong illumination model.
    vec3 lightDirection    = normalize(lightPosition - currentPosition);
    vec3 normal            = normalize(waveNormal);

    // Diffuse color.
    float diffuseIntensity = max(dot(normal, lightDirection), 0.0F);
    color += materialColor * material.y * diffuseIntensity;

    // Specular color.
    vec3 viewDirection      = normalize(camera - currentPosition);
    vec3 reflectDirection   = reflect(-lightDirection, normal);
    float specularIntensity = max(dot(reflectDirection, viewDirection), 0.0F);
    color += lightColor * material.z * pow(specularIntensity, material.w);

    fColor = vec4(color, 1.0F);
}
