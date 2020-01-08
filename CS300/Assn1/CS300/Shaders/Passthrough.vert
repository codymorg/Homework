#version 430 core

layout (location = 0) in vec3 pos;

uniform mat4 modelToWorld; // model
uniform mat4 worldToCam;   // view
uniform mat4 projection;   // projection

out vec3 normal;

void main()
{
  gl_Position = projection * worldToCam * modelToWorld * vec4(pos,1.0f);
  normal = pos + 0.5f;
}