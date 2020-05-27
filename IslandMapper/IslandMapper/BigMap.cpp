#define cimg_imagepath "Maps/"

#include "BigMap.h"
using std::string;
#include <iostream>
using std::cout;

BigMap::BigMap(std::string file) : file_(file)
{
  image_ = cimg_library::CImg<unsigned char>(file_.c_str()).normalize(0, 255).resize(-100, -100, 1, 3);
  for (int i = file_.size(); i >= 0; i--)
  {
    if (file_[i] == '.')
      debugFile_ = file_.substr(0, i) + "_out.bmp";
    if (file[i] == '/' || file[i] == '\\')
    {
      diff_ = file_.substr(0, i) + "/Diffs/" + file_.substr(i + 1);
      break;
    }
  }
}

auto BigMap::isMatch(const TreasureMap& treasure) -> std::pair<char, int>
{
#ifdef _DEBUG
  cout << "checking for island...\n";
#endif


  float maxPercentage = 0.0f;
  IslandBox& bestMatch = islandBoxes.front();
  int islandCOunt = 0;
  for (auto& island : islandBoxes)
  {
    // resize clone to island dim
    cimg_library::CImg<unsigned char> clone = treasure.getMask();
    cimg_library::CImg<unsigned char> diff = treasure.getMask();
    diff.fill(0);

    clone.resize(island.w, island.h);
    diff.resize(island.w, island.h);

    // read both - counting black pixels
    int hitCount = 0;  // big & small
    int missCount = 0; // big && !small
    for (int y = 0; y < clone.height(); y++)
    {
      for (int x = 0; x < clone.width(); x++)
      {
        hitCount += image_(island.x + x, island.y + y, 0, 0) == 0 && clone(x, y, 0, 0) == 0;
        missCount += image_(island.x + x, island.y + y, 0, 0) == 0 && clone(x, y, 0, 0) != 0;
#ifdef _DEBUG
        if (image_(island.x + x, island.y + y, 0, 0) == 0)
          diff.draw_point(x, y, blue, 1);
        if(clone(x, y, 0, 0) == 0)
          diff.draw_point(x, y, red, 1);
        if(image_(island.x + x, island.y + y, 0, 0) == 0 && clone(x, y, 0, 0) == 0)
          diff.draw_point(x, y, purple, 1);
#endif
      }
    }

    diff.save(diff_.c_str(), islandCOunt++, 2);

    float percent = hitCount / (float(hitCount) + missCount);
    if (percent > maxPercentage)
    {
      maxPercentage = percent;
      bestMatch = island;
    }
#ifdef _DEBUG
    cout << percent * 100 << "% match (" 
      << convertCoords(island.x, island.y).first << "," << convertCoords(island.x, island.y).second << "): " << islandCOunt<< "\n" ;
#endif
  }
  return convertCoords(bestMatch.x, bestMatch.y);
}

void BigMap::findIslands()
{
  // scan looking for black pixels
  for (int y = 0; y < image_.height(); y++)
  {
    for (int x = 0; x < image_.width(); x++)
    {
      // black pixel
      if (isBlack(x,y))
      {
        boxIsland(x, y);
      }
    }
  }
#ifdef _DEBUG
  image_.save(debugFile_.c_str());
#endif
}

void BigMap::drawGrid(int count)
{
  for (size_t i = 1; i < count; i++)
  {
    image_.draw_line((i / float(count)) * image_.width(), 0, (i / float(count)) * image_.width(), image_.height(), purple, 1);
    image_.draw_line(0, (i / float(count)) * image_.height(), image_.width(), (i / float(count)) * image_.height(), purple, 1);
  }
  image_.save(debugFile_.c_str());
}

// assumes monochrome

bool BigMap::isBlack(int x, int y)
{
  unsigned char color[] =
  {
    image_(x, y, 0, 0),
    image_(x, y, 0, 1),
    image_(x, y, 0, 2)
  };
  return color[0] < 16 && color[1] < 16 && color[2] < 16;
}

bool BigMap::isColor(unsigned char* color, unsigned char* min, unsigned char* max)
{
  for (size_t i = 0; i < 3; i++)
  {
    if (color[i] < min[i] || color[i] > max[i])
      return false;
  }
  return true;
}

bool BigMap::hasBlack(int startX, int startY, int count, bool alongX)
{
  if (alongX)
  {
    for (int x = startX; x < (startX + count) && x < image_.width(); x++)
    {
      if (isBlack(x, startY))
        return true;
    }
  }
  else
  {
    for (int y = startY; y < (startY + count) && y < image_.height(); y++)
    {
      if (isBlack(startX, y))
        return true;
    }
  }

  return false;
}

int BigMap::boxIsland(int xPos, int yPos)
{
  // make sure this point isn't a part of an island already
  for (auto& island : islandBoxes)
  {
    if (island.isInBox(xPos, yPos))
      return island.x + island.w;
  }
#ifdef _DEBUG
  cout << "found island " << islandBoxes.size() + 1 << " at " << "(" << xPos << "," << yPos << ")\n";
  image_.draw_circle(xPos, yPos, 2, blue, 1);
#endif

  int TL[2] = { -1, yPos };
  int BR[2] = { -1, -1 };
  
  // find farthest black pixel to the left and right
  int start = std::max(0, xPos - (boxSize / 2));
  for (int x = start; x < xPos + (boxSize / 2); x++)
  {
    bool isblack = hasBlack(x, yPos, boxSize, false);
    if (isblack)
    {
      if (TL[0] == -1)
        TL[0] = x;
      else
        BR[0] = x;
    }
  }
  //find lowest y position
  for (int y = yPos; y < (yPos + boxSize) && y < image_.height(); y++)
  {
    if (hasBlack(TL[0], y, BR[0] - TL[0], true))
      BR[1] = y;
  }
#ifdef _DEBUG
  IslandBox bounds(start, yPos, boxSize, boxSize);
  bounds.draw(green, image_);
#endif
  islandBoxes.push_back(IslandBox(TL[0], TL[1], BR[0] - TL[0], BR[1] - TL[1]));
  
  // draw for debugging
#ifdef _DEBUG
  islandBoxes.back().draw(red, image_);
#endif

  return std::max(BR[1], yPos);
}

std::pair<char, int> BigMap::convertCoords(int x, int y)
{
  return std::make_pair(char((float(x) / image_.width() * 26) + 'A'), int((float(y) / image_.height() * 26) + 1));
}


