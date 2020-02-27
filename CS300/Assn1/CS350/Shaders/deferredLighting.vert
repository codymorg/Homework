#version 430 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 fsqCoord;

out mat4 viewTrans;
out vec2 texCoord;

uniform mat4 modelToWorld; // model
uniform mat4 worldToCam;   // view

void main()
{
  viewTrans = worldToCam * modelToWorld;
  texCoord = fsqCoord;
  gl_Position = vec4(vPos,1);
}