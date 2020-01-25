#version 430 core
#define MAX_LIGHTS 16
#define PI 3.1415926535897932384626433832795


/////***** Uniform Data *****/////


uniform sampler2D texSampler0; // position
uniform sampler2D texSampler1; // normal
uniform sampler2D texSampler2; // diffuse
uniform sampler2D texSampler3; // ambient
uniform sampler2D texSampler4; // specular
uniform vec3 boundingBox[2];

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


/////***** INputs *****/////


in mat4 viewTrans;
in vec2 texCoord;


/////***** OUTputs *****/////


out vec3 fragColor;


vec3 pointLight( Light currentLight, 
                 vec3 vertPosView, 
                 vec3 normal, 
                 vec3 matDiffuse, vec3 matAmbient, vec3 matSpecular,
                 vec3 viewV)
{
  // grab light data for this light
  vec3 lightPos    = currentLight.lightPos_;
  vec3 ambient     = currentLight.ambient_;
  vec3 diffuse     = currentLight.diffuse_;
  vec3 specular    = currentLight.specular_;
  float ns         = currentLight.ns_; 
  vec3 emissive    = currentLight.emissive_;
  vec3 attenuation = currentLight.attenuation_;

  // view space conversion
  vec3 lightPosView = (viewTrans * vec4(lightPos, 1)).xyz;

  // light vector
  vec3 lightV = lightPosView - vertPosView;
  float lightMagnitude = length(lightV);
  lightV = normalize(-lightV);

  vec3 reflection = 2 * dot(normal, lightV) * normal - lightV;

  vec3 Iambient = ambient * matAmbient; // replace with material attributes

  // diffuse
  vec3 Idiffuse = diffuse * matDiffuse * max(dot(normal,lightV),0);
  vec3 Ispecular = specular * matSpecular * pow(max(dot(reflection, viewV),0), 100);

  // attenuation
  float att = min((1.0f / (attenuation.x  * lightMagnitude + attenuation.y * lightMagnitude + attenuation.z * lightMagnitude)), 1.0f);
  
  // local color
  vec3 local = att * Iambient +  att * (Idiffuse + Ispecular);
  return local;
}

void main()
{
  vec3 viewPos =            texture(texSampler0, texCoord).rgb;
  vec3 viewNorm = normalize(texture(texSampler1, texCoord).rgb);
  vec3 diffuse =            texture(texSampler2, texCoord).rgb;
  vec3 ambient =            texture(texSampler3, texCoord).rgb;
  vec3 specular =           texture(texSampler4, texCoord).rgb;

  // view vector
  vec3 viewV = -viewPos;
  viewV = normalize(viewV);

  fragColor = pointLight(lights[0], viewPos, viewNorm, diffuse, ambient, specular, viewV);
} 