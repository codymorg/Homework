#version 430 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

uniform mat4 modelToWorld; // model
uniform mat4 worldToCam;   // view
uniform mat4 projection;   // projection

out vec3 normal;
out mat4 viewTrans;
out vec3 vertPosView;

void main()
{
  mat4 viewModel = worldToCam * modelToWorld;
  gl_Position = projection * viewModel * vec4(vPos,1.0f);

  normal = normalize(mat3(transpose(inverse(viewModel))) * vNormal);
  vertPosView = (viewModel * vec4(vPos,1)).xyz;
  viewTrans = worldToCam;
}