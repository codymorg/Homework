#version 430 core
#define MAX_LIGHTS 16

uniform vec3 objColor;      // for lines
uniform int hasTexture;
uniform sampler2D texSampler;  // back  00
uniform sampler2D texSampler2;  // back  00
uniform int useGPUuv;
uniform vec3 lower;
uniform vec3 upper;
uniform int projectionType;

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
in vec2 texCoord;

const float PI = 3.1415926535897932384626433832795;


out vec3 fragColor;

vec2 CubeProjection(vec3 point2, vec3 lower2, vec3 upper2, out int face)
{
  vec2 uv;
  float absX = abs(point2.x);
  float absY = abs(point2.y);
  float absZ = abs(point2.z);
  float upperAxis = 1.0f;
  float lowerAxis = 0.0f;

  // +-X : ([z,-z],y)
  if (absX >= absY && absX >= absZ)
  {
    if (point2.x < 0.0f)
    {
      uv.x = point2.z;
      face = 3;
    }
    else
    {
      uv.x = -point2.z;
      face = 4;
    }

    uv.y = point2.y;
    upperAxis = upper2.x;
    lowerAxis = lower2.x;
  }
    
  //+-Y : (x,[z,-z])
  else if (absY >= absX && absY >= absZ)
  {
    uv.x = point2.x;

    if (point2.y < 0.0f)
    {
      uv.y = point2.z;
      face = 1;
    }
    else
    {
      uv.y = -point2.z;
      face = 5;
    }

    upperAxis = upper2.y;
    lowerAxis = lower2.y;
  }
    
  // +-Z : ([-x,x],y)
  else if (absZ >= absX && absZ >= absY)
  {
    if (point2.z < 0.0f)
    {
      uv.x = -point2.x;
      face = 0;
    }
    else
    {
      uv.x = point2.x;
      face = 2;
    }

    uv.y = point2.y;
    upperAxis = upper2.z;
    lowerAxis = lower2.z;
  }

  uv.x = (uv.x + upperAxis) / (upperAxis - lowerAxis);
  uv.y = (uv.y + upperAxis) / (upperAxis - lowerAxis);

  return uv;
}

vec2 SphereProjection(vec3 point2)
{
  vec2 uv;

  // glsl's atan returns -pi to pi 
  float theta =  atan(point2.y, point2.x) + PI;

  float r = sqrt(point2.x * point2.x + point2.y * point2.y + point2.z * point2.z);
  float phi = acos(point2.z / r);
  uv.x = theta / (2 * PI);
  uv.y = (PI - phi) / PI;

  return uv;
}

vec2 CylinderProjection(vec3 point2, vec3 lower2, vec3 upper2)
{
  vec2 uv;
  float theta = atan(point2.y, point2.x) + PI;

  float z = (point2.z - lower2.z) / (upper2.z - lower2.z);
  uv.x = theta / (2 * PI);
  uv.y = z;

  return uv;
}

vec2 GenerateUV(out int face)
{
  vec2 uv = vec2(0,0);
  vec3 point = vec3(inverse(viewModel) * vec4(vertPosView,1));
  vec3 center = vec3(lower + upper) / 2.0f;
  vec3 point2 = point - center;
  vec3 lower2 = lower - center;
  vec3 upper2 = upper - center;

  if(projectionType == 0) 
    uv = CubeProjection(point2, lower2, upper2, face);
  if(projectionType == 1) 
  {
    uv = SphereProjection(point2);
    face = -1;
  }
  if(projectionType == 2) 
  {
    uv = CylinderProjection(point2, lower2, upper2);
    face = -1;
  }

  return uv;
}

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

  vec3 texMatDiff = matDiffuse;
  vec3 texMatSpec = matSpecular;
  int face = -1;

  // textured diffuse
  if(hasTexture >= 1)
  {
    vec2 textureUV;

    if(useGPUuv == 1)
    {
      textureUV = GenerateUV(face);
    }
    else
      textureUV = texCoord;

    texMatDiff = texture( texSampler, textureUV ).rgb;

  }

  if(hasTexture == 1 && face < 0)
  {
    texMatSpec = texture( texSampler2, texCoord ).rgb;
    texMatSpec.y = texMatSpec.x;
    texMatSpec.z = texMatSpec.x;
  }

  // view space conversion
  vec3 lightPosView =   (viewTrans * vec4(lightPos, 1)).xyz;

  // light vector
  vec3 lightV;

  // override for directional type lights
  if(isDirectional)
    lightV = -currentLight.directional;
  else
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

    // textured diffuse
  if(hasTexture == 1)
    diffuse = texture( texSampler, texCoord ).rgb;

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

  float cosInner;
  float cosOuter;
  bool inInner = InsideSpot(currentLight, currentLight.spot.x, cosInner);
  bool inOuter= InsideSpot(currentLight, currentLight.spot.y, cosOuter);


  if(inInner == false && inOuter == true)
  {
    vec3 Ispecular = specular * matSpecular * pow(max(dot(reflection, viewV),0), ns);
  
    // attenuation
    float intensity = (cosOuter - currentLight.spot.y) / (currentLight.spot.x - currentLight.spot.y);
    float att = min((1.0f / (attenuation.x + attenuation.y * lightMagnitude + attenuation.z * lightMagnitude * lightMagnitude)), 1.0f);
    local = att * Iambient + (intensity * att * (Idiffuse + Ispecular));
  }

  else if(inInner)
  {
    vec3 Ispecular = specular * matSpecular * pow(max(dot(reflection, viewV),0), ns);
  
    // attenuation
    float att = min((1.0f / (attenuation.x + attenuation.y * lightMagnitude + attenuation.z * lightMagnitude * lightMagnitude)), 1.0f);
  
    // local color
    local = att * Iambient + att * (Idiffuse + Ispecular);
  }

  //outside the light totally
  else
  {
    local = 0.1 * (Iambient + Idiffuse);
  }

  return local;
}


/////***** Main *****/////

void main()
{
  fragColor = vec3(0);

  // override for lights
  if(length(objColor) > 0)
  {
    fragColor = objColor;
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
      if(lights[i].number == 0)
        break;

      // point light or directional
      if(lights[i].type < 2)
      {
        bool isDirectional = lights[i].type == 1;
        fragColor += PointLight(lights[i], viewV, isDirectional);
      }

      //spot
      if(lights[i].type == 2)
      {
        vec3 lightPosView = (viewTrans * vec4(lights[i].lightPos_, 1)).xyz;
        vec3 lightV = normalize(lightPosView - vertPosView);
        float intensity = max(dot(normal,lightV), 0.0);
        fragColor += SpotLight(lights[i], viewV);
      }
    }
  }
}