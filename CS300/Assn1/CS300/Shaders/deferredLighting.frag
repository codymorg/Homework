#version 430 core

uniform sampler2D texSampler0; // sample the textures stored on FBO1
uniform vec3 boundingBox[2];

in vec3 modelPos;

layout(location=0) out vec3 positionOut;
layout(location=1) out vec3 normalOut;

const float PI = 3.1415926535897932384626433832795;

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

vec2 GenerateUV()
{
  vec2 uv = vec2(0,0);
  vec3 point = modelPos;
  vec3 lower = boundingBox[0];
  vec3 upper = boundingBox[1];
  vec3 center = vec3(lower + upper) / 2.0f;
  vec3 point2 = point - center;
  vec3 lower2 = lower - center;
  vec3 upper2 = upper - center;

  return SphereProjection(point2);
}

void main()
{
  positionOut = texture( texSampler0, GenerateUV() ).rgb;
} 