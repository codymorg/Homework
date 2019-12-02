#version 430 core
#define MAX_LIGHTS 16

uniform vec3 objColor;      // for lines
uniform int hasTexture;
uniform sampler2D texSampler0;  // back 
uniform sampler2D texSampler1;  // down 
uniform sampler2D texSampler2;  // front
uniform sampler2D texSampler3;  // left 
uniform sampler2D texSampler4;  // right
uniform sampler2D texSampler5;  // up   
uniform int useGPUuv;
uniform vec3 lower;
uniform vec3 upper;
uniform int projectionType;

in mat4 viewModel;
in mat4 viewTrans;
in vec3 vertPosView;

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

vec2 GenerateUV(out int face)
{
  vec3 point = vec3(inverse(viewModel) * vec4(vertPosView,1));
  point = vertPosView;
  vec3 center = vec3(lower + upper) / 2.0f;
  vec3 point2 = point - center;
  vec3 lower2 = lower - center;
  vec3 upper2 = upper - center;

  return CubeProjection(point2, lower2, upper2, face);
}

void main()
{
  int face = -1;
  vec2 uv = GenerateUV(face);
  uv = vec2(1-uv.x, 1-uv.y);
  if(face == 0) fragColor = texture( texSampler0, uv ).rgb; // back 
  if(face == 1) fragColor = texture( texSampler1, uv ).rgb; // down  
  if(face == 2) fragColor = texture( texSampler2, uv ).rgb; // front 
  if(face == 3) fragColor = texture( texSampler3, uv ).rgb; // left  
  if(face == 4) fragColor = texture( texSampler4, uv ).rgb; // right 
  if(face == 5) fragColor = texture( texSampler5, uv ).rgb; // up    
}