#include "IslandBox.h"


bool IslandBox::isInBox(unsigned xPos, unsigned yPos) const
{
  return xPos >= x && xPos <= x + w && yPos >= y && yPos <= y + h;
}

void IslandBox::draw(const unsigned char* color, cimg_library::CImg<unsigned char>& image)
{
  image.draw_line(x, y, x + w, y, color, 1); // TL->TR
  image.draw_line(x + w, y, x + w, y + h, color, 1); // TR_>BR 
  image.draw_line(x, y, x, y + h, color, 1); // TL->BL
  image.draw_line(x, y + h, x + w, y + h, color, 1); // BL->BR
}