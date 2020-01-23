#version 430 core

uniform sampler2D texSampler0; // position
uniform sampler2D texSampler1; // normal
uniform vec3 boundingBox[2];

in vec3 modelPos;

out vec3 fragColor;

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
  vec3 lower = boundingBox[0];
  vec3 upper = boundingBox[1];
  vec3 center = vec3(lower + upper) / 2.0f;
  vec3 point2 = modelPos - center;

  return SphereProjection(point2);
}

void main()
{
  vec3 t0 = texture( texSampler0, GenerateUV() ).rgb;
  vec3 t1 = texture( texSampler1, GenerateUV() ).rgb;
  fragColor = t1;
} 