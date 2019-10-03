#version 400 core

in vec3 fragPos;
in vec3 normal;

uniform vec3 objColor;          // from obj
uniform vec3 lightColor;        // from light
uniform vec3 lightPos;          // from light
uniform float lightStrength;    // from light

out vec4 fragColor;

void main()
{
    // ambient
    vec3 ambient = lightStrength * lightColor; 

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir),0.0f);
    vec3 diffuse = diff * lightColor;
    vec3 newColor = (ambient + diffuse) * objColor;
        
    fragColor = vec4(newColor, 1.0f);
} 