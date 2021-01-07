#define cimg_imagepath "Maps/"

#include "BigMap.h"
using std::string;
#include <iostream>
#include <fstream>
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

auto BigMap::isMatch(const TreasureMap& treasure) -> IslandBox&
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
  return bestMatch;
}

void BigMap::findIslands()
{
  // scan each purple box
  int boxes = 8;

  for (int h = 0; h < boxes; h++)
  {
    int yStart = float(h) / boxes * image_.height();
    for (int w = 0; w < boxes; w++)
    {
      if (h == 7 && w == 7)
        break;
      int minPoint[2]{ INT_MAX, INT_MAX };
      int maxPoint[2]{ INT_MIN, INT_MIN };
      bool first = true;

      for (int y = yStart; y < ((image_.height() / boxes) + yStart); y++)
      {
        int xStart = (float(w) / boxes) * image_.width();
        int negStart = (float(w + 1) / boxes) * image_.width();
        int xCount = image_.width() / boxes;
        int blackX = hasBlack(xStart, y, xCount, true);

        if (blackX >= 0)
        {
          minPoint[0] = std::min(minPoint[0], blackX);
          minPoint[1] = std::min(minPoint[1], y);
#ifdef _DEBUG
          if (first)
          {
            image_.draw_circle(blackX, y, 2, blue, 1);
            first = false;
          }
#endif
        }

        int negX = hasBlack(negStart, y, xCount, true, true);
        if (negX >= 0)
        {
          maxPoint[0] = std::max(maxPoint[0], negX);
          maxPoint[1] = std::max(maxPoint[1], y);
        }
      }

      IslandBox box(minPoint[0], minPoint[1], maxPoint[0] - minPoint[0], maxPoint[1] - minPoint[1]);

#ifdef _DEBUG
      //cout << "found island #" << islandBoxes.size() << " in box [" << w << "," << h << "]\n";
      box.draw(green, image_);
#endif
      box.location = names[islandBoxes.size()].first;
      box.name = names[islandBoxes.size()].second;
      islandBoxes.push_back(box);
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

void BigMap::loadIslandNames()
{
  string line;
  std::ifstream myfile("Maps/locations.txt");
  if (myfile.is_open())
  {
    while (getline(myfile, line))
    {
      names.push_back(make_pair(line.substr(0, 3), line.substr(4)));
    }
    myfile.close();
  }
  else cout << "Unable to open file";
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

int BigMap::hasBlack(int startX, int startY, int count, bool alongX, bool reverse)
{
  if (alongX)
  {
    if (reverse)
    {
      for (int x = startX; x >= 0 && startX - x < count; x--)
      {
        if (isBlack(x, startY))
          return x;
      }
    }
    else
    {
      for (int x = startX; x < (startX + count) && x < image_.width(); x++)
      {
        if (isBlack(x, startY))
          return x;
      }
    }
  }
  else
  {
    for (int y = startY; y < (startY + count) && y < image_.height(); y++)
    {
      if (isBlack(startX, y))
        return y;
    }
  }

  return -1;
}



std::pair<char, int> BigMap::convertCoords(int x, int y)
{
  return std::make_pair(char((float(x) / image_.width() * 26) + 'A'), int((float(y) / image_.height() * 26) + 1));
}


