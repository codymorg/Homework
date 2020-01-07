#version 430 core

layout (location = 0) in vec3 pos;

uniform mat4 projection;
uniform mat4 camToWorld;

out vec3 color;

void main()
{
  gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
  color = pos;
}