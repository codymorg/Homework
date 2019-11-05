#version 430 core
#define MAX_LIGHTS 16

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform vec3 objColor;      // for lines



// material data
layout (std140, binding = 1) uniform material
{
  vec3 matAmbient;
  vec3 matDiffuse;
  vec3 matSpecular;
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
};

// vertex shader phong lighting
layout (std140, binding = 0) uniform lightData
{
  Light lights[MAX_LIGHTS];
};


out vec3 color;

void main()
{
  gl_Position = projection * view * model * vec4(vertPos,1.0f);

  // full phong illumination
  if(length(objColor) == 0)
  {
    color = vec3(0);

    for(int i = 0; i < MAX_LIGHTS; i++)
    {
      // grab light data for this light
      vec3 lightPos = lights[i].lightPos_;
      vec3 ambient = lights[i].ambient_;
      vec3 diffuse = lights[i].diffuse_;
      vec3 specular = lights[i].specular_;
      float ns = lights[i].ns_; 
      vec3 emissive = lights[i].emissive_;
      vec3 attenuation = lights[i].attenuation_;

      // view space conversion
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
      vec3 Iambient = ambient * matAmbient; // replace with material attributes

      // diffuse
      vec3 Idiffuse = diffuse * matDiffuse * max(dot(normal,lightV),0);

      // specular
      vec3 Ispecular = specular * matSpecular * pow(max(dot(reflection, viewV),0), 10.0);

  
      // attenuation
      float att = min((1.0f / (attenuation.x + attenuation.y * lightMagnitude + attenuation.z * lightMagnitude * lightMagnitude)), 1.0f);

      // local color
      vec3 local = att * Iambient + att * (Idiffuse + Ispecular);

      color += local;
    }
  }

  // just a light orb
  else
  {
  color = objColor;
  }
}