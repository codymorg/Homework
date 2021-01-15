#pragma once
#include <vector>
struct Color
{
public:
  float r;
  float g;
  float b;
  float a;
};

class Vertex
{
public:
  Vertex(float x, float y, float z, Color vColor);
  std::vector<int> y;
  
  Color color;

private:
};

