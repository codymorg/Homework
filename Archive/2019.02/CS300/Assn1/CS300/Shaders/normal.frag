#version 430 core



in vec3 normal;
in vec3 color;

out vec4 fragColor;

void main()
{
  fragColor = vec4(normal,1);
  fragColor = vec4(color,1);
} 