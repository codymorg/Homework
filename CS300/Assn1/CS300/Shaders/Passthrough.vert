#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 modelToWorld; // model
uniform mat4 worldToCam;   // view
uniform mat4 projection;   // projection

out vec3 norm;

void main()
{
  gl_Position = projection * worldToCam * modelToWorld * vec4(pos,1.0f);
  norm = pos + 0.5f;
}