#pragma once
#include <vector>
struct Color
{
public:
  Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
  {
  }
  float r;
  float g;
  float b;
  float a;
};

class Vertex
{
public:
  Vertex(float x, float y, float z, Color vColor) : x(x), y(y), z(z), color(vColor)
  {
  }
  
  float x, y, z;
  Color color;

private:
};

