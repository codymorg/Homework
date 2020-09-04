#ifndef _ISLANDBOX_
#define _ISLANDBOX_
#pragma once

#include "CImg-2.9.1_pre042420/CImg.h"
#include <string>

class IslandBox
{
public:
  IslandBox(unsigned xPos, unsigned yPos, unsigned width, unsigned height) : x(xPos), y(yPos), w(width), h(height) {};
  IslandBox();

  // is this point in the box
  bool isInBox(unsigned xPos, unsigned yPos) const;
  void draw(const unsigned char* color, cimg_library::CImg<unsigned char>& image);

  unsigned x;
  unsigned y;
  unsigned w;
  unsigned h;
  std::string name;
  std::string location;
};

#endif