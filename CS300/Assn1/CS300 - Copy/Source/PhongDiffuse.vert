#version 430 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform vec3 objColor;      // for lines


// vertex shader phong lighting
layout (std140, binding = 0) uniform lightData
{
  vec3 lightPos;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float ns; 
  vec3 emissive;
};




out vec3 color;

void main()
{
  gl_Position = projection * view * model * vec4(vertPos,1.0f);
  
  vec3 lightPosView =   (view * vec4(lightPos, 1)).xyz;
  vec3 vertPosView =    (view * model * vec4(vertPos, 1)).xyz;
  vec3 vertNormalView = (view * model * vec4(vertNormal, 1)).xyz;


  // light vector
  vec3 lightV = lightPosView - vertPosView;
  float lightMagnitude = length(lightV);
  lightV = normalize(lightV);

  // view vector
  vec3 viewV = - vertPosView;
  float viewDist = length(viewV);
  viewV = normalize(viewV);

  // reflect
  vec3 normal = normalize(mat3(transpose(inverse(view * model))) * vertNormal);

  vec3 Iambient = ambient; // replace with material attributes

  // diffuse
  vec3 Idiffuse = diffuse * abs(dot(normal,lightV));

  color = Iambient;
  color = emissive;
  color = Iambient + Idiffuse + emissive;
}