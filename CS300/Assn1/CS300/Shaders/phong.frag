#version 430 core
#define MAX_LIGHTS 16

uniform sampler2D texSampler;  // back  00
uniform sampler2D texSampler2;  // back  00

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
  Light lights[MAX_LIGHTS]; // all lights will be loaded before rendering
  Material material;        // material will be loaded every object
};

in vec3 normal;
in mat4 viewTrans;
in vec3 vertPosView;

const float PI = 3.1415926535897932384626433832795;


out vec3 fragColor;


/////***** Point and Directional *****/////

vec3 PointLight(Light currentLight, vec3 viewV)
{
  // grab light data for this light
  vec3 lightPos    = currentLight.lightPos_;
  vec3 ambient     = currentLight.ambient_;
  vec3 diffuse     = currentLight.diffuse_;
  vec3 specular    = currentLight.specular_;
  float ns         = currentLight.ns_; 
  vec3 emissive    = currentLight.emissive_;
  vec3 attenuation = currentLight.attenuation_;

  // grab material 
  vec3 matAmbient = material.ambient_;
  vec3 matDiffuse = material.diffuse_;
  vec3 matSpecular = material.specular_;

  // view space conversion
  vec3 lightPosView = (viewTrans * vec4(lightPos, 1)).xyz;

  // light vector
  vec3 lightV = lightPosView - vertPosView;

  float lightMagnitude = length(lightV);
  lightV = normalize(lightV);

  vec3 reflection = 2 * dot(normal, lightV) * normal - lightV;

  // ambient
  vec3 Iambient = ambient * matAmbient; 

  // diffuse
  vec3 Idiffuse = diffuse * matDiffuse * max(dot(normal,lightV),0);
  vec3 Ispecular = specular * matSpecular * pow(max(dot(reflection, viewV),0), ns);
  
  // attenuation
  float att = min((1.0f / (attenuation.x  * lightMagnitude + attenuation.y * lightMagnitude + attenuation.z * lightMagnitude)), 1.0f);

  // local color
  vec3 local = att * Iambient +  att * (Idiffuse + Ispecular);

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

  fragColor += PointLight(lights[0], viewV);
}