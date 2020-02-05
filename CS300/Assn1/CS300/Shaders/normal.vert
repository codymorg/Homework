#version 430 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

uniform mat4 modelToWorld; // model
uniform mat4 worldToCam;   // view
uniform mat4 projection;   // projection

out vec3 normal;
out vec3 color;

// material data
struct Material
{
  vec3 ambient_;
  vec3 diffuse_;
  vec3 specular_;
};

struct Light
{
  vec3 lightPos_;
  vec3 ambient_;
  vec3 diffuse_;
  vec3 specular_;
  float ns_; 
  vec3 emissive_;
  vec3 attenuation_;
  int number;
  vec3 directional;
  int type;
  vec2 spot;
};

// vertex shader phong lighting
layout (std140, binding = 0) uniform lightData
{
  Light lights[16]; // all lights will be loaded before rendering
  Material material;        // material will be loaded every object
};

void main()
{
  mat4 viewModel = worldToCam * modelToWorld;
  gl_Position = projection * viewModel * vec4(vPos,1.0f);

  normal = normalize(mat3(transpose(inverse(viewModel))) * vNormal);
  color = lights[0].diffuse_;
}