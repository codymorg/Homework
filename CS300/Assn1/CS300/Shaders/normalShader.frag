#version 400 core

in vec3 norm;

out vec4 fragColor;

void main()
{
  fragColor = vec4(norm,1);
} 