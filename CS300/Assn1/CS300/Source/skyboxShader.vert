#version 430 core
#define MAX_LIGHTS 16

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform int hasTexture;
uniform int useGPUuv;


out mat4 viewModel;
out mat4 viewTrans;
out vec3 vertPosView;


void main()
{
  viewModel = view * model;
  viewTrans = view;

  viewTrans = mat4(mat3(viewTrans));
  vec4 pos = vec4(projection * viewTrans * vec4(vertPos,1.0f));
  pos.w += 1;
  gl_Position = pos.xyww;
  
  vertPosView = (vertPos +0.5f);
}