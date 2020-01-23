#version 430 core

in vec3 position;
in vec3 normal;

layout(location=0) out vec3 positionOut;
layout(location=1) out vec3 normalOut;

void main()
{
  positionOut = position;
  normalOut = normal;
} 