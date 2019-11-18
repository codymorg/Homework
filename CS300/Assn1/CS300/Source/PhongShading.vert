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


out mat4 viewModel;
out mat4 viewTrans;
out vec3 vertPosView;
out vec3 normal;
out vec2 texCoord;


void main()
{
  viewModel = view * model;
  viewTrans = view;

  gl_Position = projection * viewModel * vec4(vertPos,1.0f);
  
  if(hasTexture == 1)
  {
    texCoord = vertUV;
  }
  else
  {
    texCoord = vec2(-1);
  }
  // view space conversion
  vertPosView =    (viewModel * vec4(vertPos, 1)).xyz;
  normal = normalize(mat3(transpose(inverse(viewModel))) * vertNormal);
}