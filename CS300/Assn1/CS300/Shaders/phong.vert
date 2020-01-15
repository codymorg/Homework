#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 modelToWorld; // model
uniform mat4 worldToCam;   // view
uniform mat4 projection;   // projection

out vec3 norm;

void main()
{
  mat4 viewModel = worldToCam * modelToWorld;
  gl_Position = projection * viewModel * vec4(pos,1.0f);
  norm = normalize(mat3(transpose(inverse(viewModel))) * normal);
}