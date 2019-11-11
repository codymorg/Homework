#version 430 core
#define MAX_LIGHTS 16

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
  int number;
  vec3 directional;
  int type;
  vec2 spot;
};

// vertex shader phong lighting
layout (std140, binding = 0) uniform lightData
{
  Light lights[MAX_LIGHTS];
};

in mat4 viewModel;
in mat4 viewTrans;
in vec3 vertPosView;
in vec3 normal;
in vec3 overRideColor;

out vec3 fragColor;

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

  // view space conversion
  vec3 lightPosView =   (viewTrans * vec4(lightPos, 1)).xyz;

  // light vector
  vec3 lightV;

  // override for directional type lights
  if(isDirectional)
    lightV = currentLight.directional;
  else
    lightV = lightPosView - vertPosView;

  float lightMagnitude = length(lightV);
  lightV = normalize(lightV);

  vec3 reflection = 2 * dot(normal, lightV) * normal - lightV;

  // ambient
  vec3 Iambient = ambient * matAmbient; // replace with material attributes

  // spot out of circle has no specular
  // diffuse
  vec3 Idiffuse = diffuse * matDiffuse * max(dot(normal,lightV),0);
  vec3 Ispecular = specular * matSpecular * pow(max(dot(reflection, viewV),0), ns);
  
  // attenuation
  float att = min((1.0f / (attenuation.x + attenuation.y * lightMagnitude + attenuation.z * lightMagnitude * lightMagnitude)), 1.0f);
  
  // directional has no attenuation component;
  if(isDirectional)
    att = 1.0f;
  
  // local color
  vec3 local = att * Iambient + att * (Idiffuse + Ispecular);

  return local;
}


/////***** Spotlight *****/////

bool InsideSpot(Light currentLight, float cosine, out float cosOut)
{
  vec3 lightPosView = (viewTrans * vec4(currentLight.lightPos_, 1)).xyz;
  vec3 lightV = normalize(lightPosView - vertPosView);

  cosOut = dot(lightV, -normalize(currentLight.directional));

  // is this fragment in the inner circle of the spotlight?
  return cosOut > cosine;
}

vec3 SpotLight(Light currentLight, vec3 viewV)
{
  vec3 local;

  // grab light data for this light
  vec3 lightPos    = currentLight.lightPos_;
  vec3 ambient     = currentLight.ambient_;
  vec3 diffuse     = currentLight.diffuse_;
  vec3 specular    = currentLight.specular_;
  float ns         = currentLight.ns_; 
  vec3 emissive    = currentLight.emissive_;
  vec3 attenuation = currentLight.attenuation_;

  // view space conversion
  vec3 lightPosView =   (viewTrans * vec4(lightPos, 1)).xyz;

  // light vector
  vec3 lightV = lightPosView - vertPosView;

  float lightMagnitude = length(lightV);
  lightV = normalize(lightV);

  vec3 reflection = 2 * dot(normal, lightV) * normal - lightV;

  // ambient
  vec3 Iambient = ambient * matAmbient;

  // diffuse
  vec3 Idiffuse = diffuse * matDiffuse * max(dot(normal,lightV),0);

  // in the light 100%
  float cosInner;
  float cosOuter;
  if(InsideSpot(currentLight, currentLight.spot.x, cosInner))
  {
    vec3 Ispecular = specular * matSpecular * pow(max(dot(reflection, viewV),0), ns);
  
    // attenuation
    float att = min((1.0f / (attenuation.x + attenuation.y * lightMagnitude + attenuation.z * lightMagnitude * lightMagnitude)), 1.0f);
  
    // local color
    local =att * Iambient + att * (Idiffuse + Ispecular);
    local = vec3(cosInner,0,0);
  }
  
  // we're inside the outer limits
  else if(InsideSpot(currentLight, currentLight.spot.y, cosOuter))
  {
    vec3 Ispecular = specular * matSpecular * pow(max(dot(reflection, viewV),0), ns);
  
    // attenuation
    float intensity = (cosInner - cosOuter) / (currentLight.spot.x - currentLight.spot.y);
    float att = min((1.0f / (attenuation.x + attenuation.y * lightMagnitude + attenuation.z * lightMagnitude * lightMagnitude)), 1.0f);
    local = intensity * (att * Iambient + att * (Idiffuse + Ispecular));
    local = vec3(0,intensity,0);
  }

  //outside the light totally
  else
  {
    local = 0.1 * (Iambient + Idiffuse);
    local = vec3(0,0,1);
  }

  return local;
}

void main()
{
  // override for lights
  if(length(objColor) > 0)
  {
    fragColor = objColor;
    fragColor = vec3(1,0,0);
  }
  // phong shading
  else
  {
    // view vector
    vec3 viewV = -vertPosView;
    float viewDist = length(viewV);
    viewV = normalize(viewV);

    for(uint i = 0; i < MAX_LIGHTS; i++)
    {
      if(lights[i].number >1)
        break;

      // point light or directional
//      if(lights[i].type < 2)
//      {
//        bool isDirectional = lights[i].type == 0;
//        fragColor += PointLight(lights[i], viewV, isDirectional);
//      }

      //spot
      if(lights[i].type == 0)
      {
        vec3 lightPosView = (viewTrans * vec4(lights[i].lightPos_, 1)).xyz;
        vec3 lightV = normalize(lightPosView - vertPosView);
        float intensity = max(dot(normal,lightV), 0.0);
        fragColor += SpotLight(lights[i], viewV);
      }
    }
  }
}