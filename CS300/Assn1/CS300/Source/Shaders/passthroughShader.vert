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

  vec3 reflection = 2 * dot(normal, lightV) * normal - lightV;

  // ambient
  vec3 Iambient = ambient * vec3(1); // replace with material attributes

  // diffuse
  vec3 Idiffuse = diffuse * vec3(1) * max(dot(normal,lightV),0);

  // specular
  vec3 Ispecular = specular * vec3(1) * pow(max(dot(reflection, viewV),0), 1.0);
  //if(length(Idiffuse) == 0)
    //Ispecular = vec3(0,0,0);

  // local color
  vec3 local = (Iambient + Idiffuse + Ispecular) + emissive;

  color = reflection;
}