#version 400 core

in vec3 fragPos;
in vec3 normal;

uniform vec3 color;
uniform vec3 lightColor;
uniform vec3 lightPos;

out vec4 fragColor;

void main()
{
    // ambient
    vec3 ambient = 0.1 * lightColor; 

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir),0.0f);
    vec3 diffuse = diff * lightColor;
    vec3 newColor = (ambient + diffuse) * color;

    fragColor = vec4(newColor, 1.0f);
    //fragColor = vec4(normal,1.0f);
    
} 