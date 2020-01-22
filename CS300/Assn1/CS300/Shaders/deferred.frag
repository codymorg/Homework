#version 430 core

in vec3 position;
in vec3 normal;

layout(location=1) out vec3 positionOut;
layout(location=0) out vec3 normalOut;

void main()
{
  positionOut = vec3(1,0,0);
  normalOut = vec3(0,0,1);
} 