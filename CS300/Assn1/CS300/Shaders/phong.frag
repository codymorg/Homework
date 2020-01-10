#version 430 core
#define MAX_LIGHTS 16

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

struct Material
{
  vec3 matAmbient;
  vec3 matDiffuse;
  vec3 matSpecular;
};

// vertex shader phong lighting
layout (std140, binding = 0) uniform lightData
{
  Light lights[MAX_LIGHTS];
  Material material;
};

in vec3 normal;
in mat4 viewTrans;
const float PI = 3.1415926535897932384626433832795;

out vec3 fragColor;


/////***** Point and Directional *****/////

vec3 PointLight(Light currentLight, vec3 viewV, bool isDirectional)
{
  // grab light data for this light
  vec3 lightPos    = currentLight.lightPos_;
  vec3 ambient     = currentLight.ambient_;
  vec3 diffuse     = currentLight.diffuse_;
  vec3 specular    = currentLight.specular_;
  float ns         = currentLight.ns_; 
  vec3 emissive    = currentLight.emissive_;
  vec3 attenuation = currentLight.attenuation_;

  vec3 texMatDiff = material.matDiffuse;
  vec3 texMatSpec = material.matSpecular;
  int face = -1;

  // view space conversion
  vec3 lightPosView =   (viewTrans * vec4(lightPos, 1)).xyz;

  // light vector
  vec3 lightV;

  // override for directional type lights
  lightV = lightPosView - vertPosView;

  float lightMagnitude = length(lightV);
  lightV = normalize(lightV);

  vec3 reflection = 2 * dot(normal, lightV) * normal - lightV;

  // ambient
  vec3 Iambient = ambient * matAmbient; // replace with material attributes

  // diffuse
  vec3 Idiffuse = diffuse * texMatDiff * max(dot(normal,lightV),0);
  vec3 Ispecular = specular * texMatSpec * pow(max(dot(reflection, viewV),0), ns);
  
  // attenuation
  float att = min((1.0f / (attenuation.x + attenuation.y * lightMagnitude + attenuation.z * lightMagnitude * lightMagnitude)), 1.0f);
  
  // directional has no attenuation component;
  if(isDirectional)
    att = 1.0f;
  
  // local color
  vec3 local = att * Iambient + att * (Idiffuse + Ispecular);
  return local;
}



/////***** Main *****/////

void main()
{
  fragColor = vec3(0);

  // view vector
  vec3 viewV = -vertPosView;
  float viewDist = length(viewV);
  viewV = normalize(viewV);

  for(uint i = 0; i < MAX_LIGHTS; i++)
  {
    if(lights[i].number == 0)
      break;

    fragColor += PointLight(lights[i], viewV, isDirectional);
  }
}
