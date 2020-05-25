#ifndef _ISLANDBOX_
#define _ISLANDBOX_
#pragma once

#include "CImg-2.9.1_pre042420/CImg.h"

class IslandBox
{
public:
  IslandBox(unsigned xPos, unsigned yPos, unsigned width, unsigned height) : x(xPos), y(yPos), w(width), h(height) {};

  bool isInBox(unsigned xPos, unsigned yPos) const;
  void draw(const unsigned char* color, cimg_library::CImg<unsigned char>& image);

  unsigned x;
  unsigned y;
  unsigned w;
  unsigned h;
};

#endif