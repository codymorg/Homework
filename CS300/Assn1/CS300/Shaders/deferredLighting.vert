#version 430 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

uniform mat4 modelToWorld; // model
uniform mat4 worldToCam;   // view
uniform mat4 projection;   // projection

out vec3 modelPos;

void main()
{
  mat4 modelToCam = worldToCam * modelToWorld;
  gl_Position = projection * modelToCam * vec4(vPos,1.0f);
  modelPos = vPos;
}