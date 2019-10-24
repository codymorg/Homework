#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

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
  gl_Position = projection * view * model * vec4(aPos,1.0f);

  // light vector
  vec3 lightV = lightPos - aPos;
  float lightMagnitude = length(lightV);
  lightV = normalize(lightV);
  color = lightV;

  // view vector
  vec3 viewV = cameraPos - aPos;
  float viewDist = length(viewV);
  viewV = normalize(viewV);

  // reflect
  vec3 normal = normalize(mat3(transpose(inverse(model))) * aNormal);

  vec3 reflection = 2 * dot(normal, lightV) * normal - lightV;

  // ambient
  vec3 Iambient = ambient * vec3(1); // replace with material attributes

  // diffuse
  vec3 Idiffuse = diffuse * vec3(1) * max(dot(normal,lightV),0);

  // specular
  vec3 Ispecular = specular * vec3(1) * pow(max(dot(reflection, viewV), 0), 1);

  // local color
  vec3 local = (Iambient + Idiffuse + Ispecular) + emissive;

  // add fog and att?
  /*
  color = reflection;
  color = Iambient;
  color = Idiffuse;
  color = emissive;
  color = local;
  */
  color = Ispecular;
}