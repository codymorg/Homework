#version 430 core

in vec3 vPosView;
in vec3 normal;
in vec3 diffuse;
in vec3 ambient;
in vec3 specular;

layout(location=0) out vec3 viewPosOut;
layout(location=1) out vec3 viewNormalOut;
layout(location=2) out vec3 diffuseOut;
layout(location=3) out vec3 ambientOut;
layout(location=4) out vec3 specularOut;

void main()
{
  viewPosOut = vPosView;
  viewNormalOut = normalize(normal);
  diffuseOut = diffuse;
  ambientOut = ambient;
  specularOut = specular;
} 